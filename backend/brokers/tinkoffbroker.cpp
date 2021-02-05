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


#include "tinkoffbroker.h"
#include "backend/controller/preferences.h"
#include "frontend/alert/alert.h"

#include <QtNetwork/QSslError>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include <QtCore/QDebug>


TinkoffBroker::TinkoffBroker(QObject *parent)
	: AbstractBroker(parent)
{
	m_manager = new QNetworkAccessManager(this);
	m_token = preferences.read()->credentials;

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
	candle["interval"] = QString("%1min").arg(m_intervalReq);

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

	if (parseError.error != QJsonParseError::NoError)
		Alert().up("Json Parse Error", parseError.errorString());
	
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
	Alert().up("SSL Errors", errorsText);
}

void TinkoffBroker::requestStocks()
{
	m_manager->get(authRequest(m_stocksUrl));
}

void TinkoffBroker::requestCandles(QString figi, int interval)
{
	QUrl candlesUrl(m_candlesUrl);

	auto to = QDateTime::currentDateTimeUtc();
	// One day is the maximum allowed range by Tinkoff API
	// for 1, 5, 15, 30 mins intervals
	QDateTime from(to);
	from = from.addDays(-1);
	from = from.addSecs(5);

	QString query = "figi=%1&from=%2&to=%3&interval=%4min";
	query = query.arg(figi).arg(
		from.toString(Qt::ISODateWithMs)).arg(
		to.toString(Qt::ISODateWithMs)).arg(interval);

	candlesUrl.setQuery(query);
	
	m_manager->get(authRequest(candlesUrl));
};

void TinkoffBroker::subscribe()
{
	m_webSocket.close();
	m_webSocket.open(authRequest(m_websocketUrl));
}

QNetworkRequest TinkoffBroker::authRequest(QUrl url)
{
	if (m_token.isEmpty())
		Alert().up("Sandbox token is empty. Preferences: Credentials");
	auto request = QNetworkRequest(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	auto bearer = QString("Bearer %1").arg(m_token);
	request.setRawHeader("Authorization", bearer.toLatin1());
	return request;
}

void TinkoffBroker::responseReceived(QNetworkReply *reply)
{

	auto data = reply->readAll();

	auto networkError = reply->error();
	if (networkError != QNetworkReply::NoError)
		Alert().up("Network Error", data);

	QJsonParseError parseError;
    auto json = QJsonDocument::fromJson(data, &parseError);

	if (parseError.error != QJsonParseError::NoError)
		Alert().up("Json Parse Error", parseError.errorString());

	auto url = reply->url();

	if (url.matches(m_stocksUrl, QUrl::RemoveQuery))
		parseStocks(json);
	else if (url.matches(m_candlesUrl, QUrl::RemoveQuery))
		parseCandles(json);

}

void TinkoffBroker::parseOrderBook(QJsonObject json)
{
	// TODO probably causes segfaults check lengths
	auto bid = json["bids"].toArray()[0].toArray()[0].toDouble();
	auto ask = json["asks"].toArray()[0].toArray()[0].toDouble();

	double price = (bid + ask) / 2;
	
	emit sendBidAskPrice(price);
}

void TinkoffBroker::parseCandle(QJsonObject json)
{
	Candle c;

	c.open = json["o"].toDouble();
	c.close = json["c"].toDouble();
	c.high = json["h"].toDouble();
	c.low = json["l"].toDouble();
	c.volume = json["v"].toInt();

	auto time = json["time"].toString();
	auto timeStamp = QDateTime::fromString(time, Qt::ISODate);

	c.time = timeStamp;

	//int interval = json["interval"].toString().replace("min", "").toInt();

	emit sendLatestCandle(c);
}

void TinkoffBroker::parseCandles(QJsonDocument json)
{
	QList<Candle *> candles;

	const auto jsonCandles = json["payload"]["candles"].toArray();

	// TODO different hours for different exchanges
	auto open = QTime::fromString("14:30", "hh:mm");
	auto close = QTime::fromString("21:00", "hh:mm");

	double prevClosePrice = -1;

	QString intervalString;
	QString figi;
	for (auto jc : jsonCandles) {
		auto *c = new Candle;

		c->open = jc["o"].toDouble();
		c->close = jc["c"].toDouble();
		c->high = jc["h"].toDouble();
		c->low = jc["l"].toDouble();
		c->volume = jc["v"].toInt();

		auto tsS = jc["time"].toString();
		auto tsD = QDateTime::fromString(tsS, Qt::ISODate);

		if (prevClosePrice < 0 && tsD.time() >= close)
			prevClosePrice = c->close;

		c->time = tsD;

		candles.append(c);

		intervalString = jc["interval"].toString();
		figi = jc["figi"].toString();
	}
	
	if (prevClosePrice > -1)
		emit sendPrevClose(prevClosePrice);

	int interval = intervalString.replace("min", "").toInt();

	emit sendCandles(candles, interval);

	emit sendExchangeHours(open, close);

	m_intervalReq = interval;
	m_figiReq = figi;
	subscribe();
}


void TinkoffBroker::parseStocks(QJsonDocument json)
{
	QList<Stock *> stocks;

	const auto instruments = json["payload"]["instruments"].toArray();

	for (auto i : instruments) {
		if (i["currency"] != "USD")
			continue;
		auto *s = new Stock;
		s->ticker = i["ticker"].toString();
		s->name = i["name"].toString();
		s->figi = i["figi"].toString();
		stocks.append(s);
	}
	emit sendStocks(stocks);
}

void TinkoffBroker::brokerChanged()
{
	m_webSocket.close();
};


