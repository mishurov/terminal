/****************************************************************************
**
** This file is part of the Blackfriars Trading Terminal
**
** Copyright (C) 2021 Alexander Mishurov
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU
** General Public License version 3. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/


#include "TinkoffBroker.h"

#include "frontend/Blackfriars/Alert.h"
#include "frontend/Blackfriars/Facade.h"
#include "backend/Controller/Controller.h"

#include <QtNetwork/QSslError>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include <QtCore/QDebug>


//1min, 2min, 3min, 5min, 10min, 15min, 30min, hour, day, week, month
#define INTERVALS { 1, 2, 3, 5, 10, 15, 30, 60, 1440, 10080, 99999 }


TinkoffBroker::TinkoffBroker(Controller *parent)
	: AbstractBroker(parent)
{
	m_manager = new QNetworkAccessManager(this);

	auto s = this->parent()->facade()->settings();
	const Preferences &p = this->parent()->facade()->preferences();
	m_token = s->value(p.credentialsKey(), "").toString();

	// TODO: move URLs to config constants
	auto api = QString("https://api-invest.tinkoff.ru/openapi/sandbox");
	m_stocksUrl = QUrl(api + "/market/stocks");
	m_candlesUrl = QUrl(api + "/market/candles");

	connect(m_manager, &QNetworkAccessManager::finished, this, &TinkoffBroker::responseReceived);

	m_websocketUrl = QUrl("wss://api-invest.tinkoff.ru/openapi/md/v1/md-openapi/ws");

	connect(&m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
		this, &TinkoffBroker::onSslErrors);
	connect(&m_webSocket, &QWebSocket::connected, this, &TinkoffBroker::onConnected);
	connect(&m_webSocket, &QWebSocket::textMessageReceived,
		this, &TinkoffBroker::onTextMessageReceived);
}


TinkoffBroker::~TinkoffBroker()
{
	m_webSocket.close();
}


void TinkoffBroker::onConnected()
{
	QJsonObject candle;
	candle["event"] = "candle:subscribe";
	candle["figi"] = m_figiReq;
	candle["interval"] = intervalToString(m_intervalReq);

	QJsonDocument docCandle(candle);
	QByteArray docCandleByteArray = docCandle.toJson(QJsonDocument::Compact);
	m_webSocket.sendBinaryMessage(docCandleByteArray);

	QJsonObject orderbook;
	orderbook["event"] = "orderbook:subscribe";
	orderbook["figi"] = m_figiReq;
	orderbook["depth"] = 1;

	QJsonDocument docOrderBook(orderbook);
	QByteArray docOrderBookByteArray = docOrderBook.toJson(QJsonDocument::Compact);
	m_webSocket.sendBinaryMessage(docOrderBookByteArray);
}


void TinkoffBroker::onTextMessageReceived(const QString &message)
{
	QJsonParseError parseError;
    auto json = QJsonDocument::fromJson(message.toLatin1(), &parseError);

	if (parseError.error != QJsonParseError::NoError) {
		qDebug() << "Json Parse Error" << parseError.errorString();
		Alert().up("Json Parse Error", parseError.errorString());
	}
	
	auto event = json["event"].toString();

	if (event == "candle")
		parseCandle(json["payload"].toObject());
	else if (event == "orderbook")
		parseOrderBook(json["payload"].toObject());
}


void TinkoffBroker::onSslErrors(const QList<QSslError> &errors)
{
	QString errorsText;
	for (auto e : errors)
		errorsText += e.errorString();
	qDebug() << "SSL Errors" << errorsText;
	Alert().up("SSL Errors", errorsText);
}


void TinkoffBroker::requestBrokerData()
{
	m_manager->get(authRequest(m_stocksUrl));
}


void TinkoffBroker::requestCandles(QString figi, QDateTime to, int interval, int count)
{
	isSubsequentRequest = false;
	if (m_figiReq != figi || interval != m_intervalReq) {
		m_recievedCandles.clear();
		hasNoCandles = false;
		QDateTime prevDayTo = to.addDays(-1);
		isPrevDayRequest = true;
		makeCandlesRequest(figi, prevDayTo, 1440);
	} else if (hasNoCandles) {
		// ignore if the broker has no more historical data
		return;
	}

	// check if there're candles from the last request
	int receivedSize = m_recievedCandles.size();
	if (receivedSize && m_recievedCandles.last()->time == to) {
		if (receivedSize >= count) {
			emit sendCandles(m_recievedCandles.mid(receivedSize - count, count));
			m_recievedCandles.remove(receivedSize - count, count);
			return;
		} else {
			isSubsequentRequest = true;
			to = m_recievedCandles.first()->time;
			to = interval < 99999 ? to.addSecs(-interval * 60) : to.addMonths(-1);
		}
	}
	m_countReq = count;
	makeCandlesRequest(figi, to, interval);
};


void TinkoffBroker::makeCandlesRequest(QString figi, QDateTime to, int interval)
{
	QUrl candlesUrl(m_candlesUrl);

	// fix if "to" is a weekend, to avoid empty candles
	// TODO: other non-trading days
	if (to.date().dayOfWeek() > 5) {
		to = to.addDays(-to.date().dayOfWeek() + 6);
		auto time = to.time();
		time.setHMS(0, 0, 0, 0);
		to.setTime(time);
	}

	to = to.addSecs(1);

	QDateTime from = intervalToStartDate(to, interval);
	QString intervalName = intervalToString(interval);

	QString query = "figi=%1&from=%2&to=%3&interval=";
	query = query.arg(figi).arg(
		from.toString(Qt::ISODateWithMs)).arg(
		to.toString(Qt::ISODateWithMs));
	query += intervalName;

	candlesUrl.setQuery(query);

	m_manager->get(authRequest(candlesUrl));
}


void TinkoffBroker::subscribe()
{
	m_webSocket.close();
	m_webSocket.open(authRequest(m_websocketUrl));
}


QNetworkRequest TinkoffBroker::authRequest(QUrl url)
{
	if (m_token.isEmpty()) {
		QString msg = "Sandbox token is empty. Preferences: Credentials";
		qDebug() << msg;
		Alert().up(msg);
	}
	auto request = QNetworkRequest(url);
	request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	auto bearer = QString("Bearer %1").arg(m_token);
	request.setRawHeader("Authorization", bearer.toLatin1());
	return request;
}


void TinkoffBroker::responseReceived(QNetworkReply *reply)
{
	QString msg;

	auto data = reply->readAll();

	auto networkError = reply->error();
	if (networkError != QNetworkReply::NoError) {
		if (networkError == QNetworkReply::AuthenticationRequiredError)
			msg = "Authentication Error";
		else
			msg = "Network Error";
		qDebug() << msg << data;
		Alert().up(msg, data);
	}

	QJsonParseError parseError;
    auto json = QJsonDocument::fromJson(data, &parseError);

	if (parseError.error != QJsonParseError::NoError) {
		msg = "Json Parse Error";
		qDebug() << msg << parseError.errorString();
		Alert().up(msg, parseError.errorString());
	}

	auto url = reply->url();

	if (url.matches(m_stocksUrl, QUrl::RemoveQuery))
		parseStocks(json);
	else if (url.matches(m_candlesUrl, QUrl::RemoveQuery))
		parseCandles(json);

}


void TinkoffBroker::parseOrderBook(QJsonObject json)
{
	if (!json["bids"].toArray().size()
	    || !json["bids"].toArray()[0].toArray().size()
	    || !json["asks"].toArray().size()
	    || !json["asks"].toArray()[0].toArray().size())
		return;

	auto bid = json["bids"].toArray()[0].toArray()[0].toDouble();
	auto ask = json["asks"].toArray()[0].toArray()[0].toDouble();

	qreal price = (bid + ask) / 2;

	emit sendBidAskAverage(price);
}


void TinkoffBroker::parseCandle(QJsonObject json)
{
	QFCandle c;

	c.open = json["o"].toDouble();
	c.close = json["c"].toDouble();
	c.high = json["h"].toDouble();
	c.low = json["l"].toDouble();
	c.volume = json["v"].toInt();

	auto time = json["time"].toString();
	auto timeStamp = QDateTime::fromString(time, Qt::ISODate);

	c.time = timeStamp;

	// TODO: check if consistent with requested interval
	//int interval = intervalFromString(json["interval"].toString());

	emit sendLatestCandle(c);
}


void TinkoffBroker::parseCandles(QJsonDocument json)
{
	const auto jsonCandles = json["payload"]["candles"].toArray();

	if (!isSubsequentRequest && !jsonCandles.size()) {
		QString msg = "No candles received";
		qDebug() << msg;
		Alert().up(msg);
		return;
	}

	if (isPrevDayRequest && jsonCandles.size() && jsonCandles.at(0)["interval"] == "day") {
		isPrevDayRequest = false;
		emit sendPrevDayClose(jsonCandles.last()["c"].toDouble());
		return;
	}

	if (jsonCandles.size()) {
		QString intervalString = jsonCandles.at(0)["interval"].toString();
		QString figi = jsonCandles.at(0)["figi"].toString();
		// TODO: check if consistent with requested interval
		int interval = intervalFromString(intervalString);
		if (m_intervalReq != interval || m_figiReq != figi)
			subscribe();
		m_intervalReq = interval;
		m_figiReq = figi;
	}

	QList<QFCandle *> candles;

	for (int i = 0; i < jsonCandles.size(); i++) {
		const QJsonValue jc = jsonCandles.at(i);
		auto *c = new QFCandle;

		c->open = jc["o"].toDouble();
		c->close = jc["c"].toDouble();
		c->high = jc["h"].toDouble();
		c->low = jc["l"].toDouble();
		c->volume = jc["v"].toInt();

		auto tsS = jc["time"].toString();
		auto tsD = QDateTime::fromString(tsS, Qt::ISODate);

		// Tinkoff tends to skip candles in non-trading ranges,
		// fill it with flat candles to keep the timelime consistent
		if (m_intervalReq < 5 && i > 0) {
			const QJsonValue jcp = jsonCandles.at(i - 1);
			auto tsSP = jcp["time"].toString();
			auto tsDP = QDateTime::fromString(tsSP, Qt::ISODate);
			int timeDiff = floor(tsDP.secsTo(tsD) / m_intervalReq / 60);
			if (tsDP.date() == tsD.date() && timeDiff > 1) {
				for (int j = 1; j < timeDiff; j++) {
					auto *ca = new QFCandle;
					ca->open = jcp["c"].toDouble();
					ca->close = ca->open;
					ca->high = ca->open;
					ca->low = ca->open;
					ca->volume = 0;
					ca->time = tsDP.addSecs(j * m_intervalReq * 60);
					candles.push_back(ca);
				}
			}
		}

		c->time = tsD;
		candles.push_back(c);
	}

	m_recievedCandles = candles + m_recievedCandles;

	int receivedSize = m_recievedCandles.size();

	if (candles.size() && receivedSize < m_countReq) {
		QDateTime to = m_recievedCandles.first()->time;
		to = m_intervalReq < 99999 ? to.addSecs(-m_intervalReq * 60) : to.addMonths(-1);
		isSubsequentRequest = true;
		makeCandlesRequest(m_figiReq, to, m_intervalReq);
		return;
	}

	int numToSend = m_countReq;
	if (!candles.size()) {
		// send what is collected and mark that the broker has no more data
		numToSend = receivedSize;
		hasNoCandles = true;
	}

	emit sendCandles(m_recievedCandles.mid(receivedSize - numToSend, numToSend));
	m_recievedCandles.remove(receivedSize - numToSend, numToSend);
}


void TinkoffBroker::parseStocks(QJsonDocument json)
{
	QList<Stock *> stocks;

	const auto instruments = json["payload"]["instruments"].toArray();

	for (const QJsonValue i : instruments) {
		if (i["currency"] != "USD")
			continue;
		auto *s = new Stock;
		s->ticker = i["ticker"].toString();
		s->name = i["name"].toString();
		s->figi = i["figi"].toString();
		stocks.append(s);
	}
	emit sendBrokerData(stocks, INTERVALS);
}


void TinkoffBroker::brokerChanged()
{
	m_webSocket.close();
};


QString TinkoffBroker::intervalToString(int interval)
{
	QString intervalName;

	if (interval < 60)
		intervalName = QString("%1min").arg(interval);
	else if (interval == 60)
		intervalName = "hour";
	else if (interval == 1440)
		intervalName = "day";
	else if (interval == 10080)
		intervalName = "week";
	else // 99999
		intervalName = "month";

	return intervalName;
}


int TinkoffBroker::intervalFromString(QString intervalName)
{
	int interval;

	if (intervalName.contains("min"))
		interval = intervalName.replace("min", "").toInt();
	else if (intervalName == "hour")
		interval = 60;
	else if (intervalName == "day")
		interval = 1440;
	else if (intervalName == "week")
		interval = 10080;
	else // month
		interval = 99999;

	return interval;
}


QDateTime TinkoffBroker::intervalToStartDate(QDateTime to, int interval) {
	QDateTime from(to);

	if (interval < 60)
		from = from.addDays(-1);
	else if (interval == 60)
		from = from.addDays(-7);
	else if (interval == 1440)
		from = from.addMonths(-11);
	else if (interval == 10080)
		from = from.addMonths(-23);
	else // 99999 month
		from = from.addYears(-10);

	return from;
}
