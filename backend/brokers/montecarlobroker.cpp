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


#include "montecarlobroker.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QTimer>

#include <QtCore/QDebug>


MonteCarloBroker::MonteCarloBroker(QObject *parent)
	: AbstractBroker(parent),
	m_engine((std::random_device())())
{
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &MonteCarloBroker::genCandle);

	m_gausMean = 60;
	m_gausDist = std::normal_distribution<double>{m_gausMean,7};
	m_expDist = std::exponential_distribution<double>(1.5);
}

void MonteCarloBroker::requestStocks()
{
	QList<Stock *> stocks;

	QFile file(":/tinkoff_stocks.json");
	if (!file.open(QFile::ReadOnly)) {
		emit sendStocks(stocks);
		return;
	}

	auto json = QJsonDocument::fromJson(file.readAll());
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

	emit sendStocks(stocks);
}

void MonteCarloBroker::genCandle()
{
	auto now = QDateTime::currentDateTimeUtc();
	int seconds = 60 * m_interval;

	if (m_lastCandle.time.secsTo(now) > seconds) {
		m_lastCandle.time = m_lastCandle.time.addSecs(seconds);
		m_lastCandle.open = m_lastCandle.close;
		m_lastCandle.low = 	m_lastCandle.close;
		m_lastCandle.high = m_lastCandle.close;
		return;
	}

	double v = m_lastCandle.volume + (m_gausDist(m_engine) - m_gausMean) * 0.5;
	v = qMin(10.0, v);
	m_lastCandle.volume = v;
	
	int numSamples = 30;
	double samples[numSamples];

	samples[0] = m_lastCandle.close;

	double l = 999999;
	double h = 0;
	double c;
	for (int i = 1; i < numSamples; i++) {
		samples[i] = samples[i - 1] + (m_gausDist(m_engine) - m_gausMean) * 0.05;
		l = qMin(l, samples[i]);
		h = qMax(h, samples[i]);
	}
	c = samples[numSamples - 1];
	if (l < 0) {
		h -= l;
		c -= l;
		l = 0;
	}

	m_lastCandle.low = qMin(l, m_lastCandle.low);
	m_lastCandle.high = qMax(h, m_lastCandle.high);
	m_lastCandle.close = c;
	
	emit sendBidAskPrice(c);

	emit sendLatestCandle(m_lastCandle);
}


void MonteCarloBroker::requestCandles(QString figi, int interval)
{
	m_timer->stop();

	QList<Candle *> candles;

	auto close = QTime::fromString("21:00", "hh:mm");
	auto open = QTime::fromString("14:30", "hh:mm");

	auto to = QDateTime::currentDateTimeUtc();
	
	// adjust time to interval
	auto now = to.time();
	int hour = now.hour();
	int minute = now.minute();

	while (minute % interval)
		minute--;

	now.setHMS(hour, minute, 0, 0);
	to.setTime(now);

	QDateTime from(to);
	from = from.addDays(-1);

	auto seconds = from.secsTo(to);
	int num = seconds / 60 / interval;

	int bucket = 10;
	int numSamples = num * bucket;
	double samples[numSamples];
	
	samples[0] = m_gausMean * 11;

	double min = 0;
	for (int i = 1; i < numSamples; i++) {
		samples[i] = samples[i - 1] + m_gausDist(m_engine) - m_gausMean;
		min = qMin(min, samples[i]);
	}
	if (min < 0) {
		for (int i = 1; i < numSamples; i++)
			samples[i] -= min;
	}

	QDateTime ts = from;
	int offset;
	double o, c, h, l, v;
	double closePrice = -1;

	for (int i = 0; i < num; i++) {
		v = m_expDist(m_engine) * 200;
		if (ts.time() < open || ts.time() > close)
			v *= 0.1;

		offset = i * bucket;
		o = samples[offset];
		c = samples[offset + bucket - 1];
		h = 0;
		l = 999999;
		for (int j = 0; j < bucket; j++) {
			double s = samples[offset + j];
			h = qMax(h, s);
			l = qMin(l, s);
		}

		auto *candle = new Candle;

		candle->open = o;
		candle->close = c;
		candle->high = h;
		candle->low = l;
		candle->volume = v;
		candle->time = ts;

		candles.append(candle);

		if (closePrice < 0 && ts.time() >= close)
			closePrice = candle->close;

		ts = ts.addSecs(60 * interval);
	}

	if (closePrice > -1)
		emit sendPrevClose(closePrice);
	
	emit sendCandles(candles, interval);

	emit sendExchangeHours(open, close);

	m_lastCandle = *candles.last();

	m_interval = interval;
	m_timer->start(1000);
};


void MonteCarloBroker::brokerChanged()
{
	m_timer->stop();
};
