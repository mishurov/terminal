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


#include "MonteCarloBroker.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QTimer>


#define INTERVALS { 1, 2, 3, 5, 10, 15, 30, 60, 1440, 10080, 99999 }


MonteCarloBroker::MonteCarloBroker(Controller *parent)
	: AbstractBroker(parent),
	m_engine((std::random_device())())
{
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &MonteCarloBroker::genCandle);

	m_gausMean = 60;
	m_gausDist = std::normal_distribution<qreal>{m_gausMean,7};
	m_expDist = std::exponential_distribution<qreal>(1.5);
}


void MonteCarloBroker::requestBrokerData()
{
	QList<Stock *> stocks;

	QFile file(":/tinkoff_stocks.json");
	if (!file.open(QFile::ReadOnly)) {
		emit sendBrokerData(stocks, INTERVALS);
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

	emit sendBrokerData(stocks, INTERVALS);
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

	qreal v = m_lastCandle.volume + (m_gausDist(m_engine) - m_gausMean) * 0.5;
	v = qMax(10.0, v);
	m_lastCandle.volume = v;
	
	int numSamples = 30;
	qreal samples[numSamples];

	samples[0] = m_lastCandle.close;

	qreal l = 999999;
	qreal h = 0;
	qreal c;
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
	
	emit sendLatestCandle(m_lastCandle);
	emit sendBidAskAverage(m_lastCandle.close);
}


void MonteCarloBroker::requestCandles(QString figi, QDateTime to, int interval, int count)
{
	if (figi != m_figi || interval != m_interval)
		m_timer->stop();

	QList<QFCandle *> candles;

	auto now = to.time();
	int hour = now.hour();
	int minute = now.minute();

	while (minute % interval)
		minute--;

	if (interval < 60)
		now.setHMS(hour, minute, 0, 0);
	else if (interval < 1440)
		now.setHMS(hour, 0, 0, 0);
	else
		now.setHMS(0, 0, 0, 0);

	to.setTime(now);

	int bucket = 10;
	int numSamples = (count + 1) * bucket;
	qreal samples[numSamples];
	
	samples[0] = qIsNaN(m_prevSample) ? m_gausMean * 11 : m_prevSample;

	qreal min = 0;
	for (int i = 1; i < numSamples; i++) {
		samples[i] = samples[i - 1] + m_gausDist(m_engine) - m_gausMean;
		min = qMin(min, samples[i]);
	}
	if (min < 0) {
		for (int i = 1; i < numSamples; i++)
			samples[i] -= min;
	}

	QDateTime ts = to;
	int offset;
	qreal o, c, h, l, v;

	for (int i = 1; i <= count; i++) {
		v = m_expDist(m_engine) * 200;

		offset = i * bucket;
		o = samples[offset];
		c = samples[offset + bucket - 1];
		h = 0;
		l = 999999;
		for (int j = 0; j < bucket; j++) {
			qreal s = samples[offset + j];
			h = qMax(h, s);
			l = qMin(l, s);
		}

		if (i == count)
			m_prevSample = o;

		auto *candle = new QFCandle;

		candle->open = o;
		candle->close = c;
		candle->high = h;
		candle->low = l;
		candle->volume = v;
		candle->time = ts;

		candles.append(candle);

		if (interval < 1440)
			ts = ts.addSecs(-60 * interval);
		else if (interval == 1440)
			ts = ts.addDays(-1);
		else if (interval == 10080)
			ts = ts.addDays(-7);
		else
			ts = ts.addMonths(-1);
	}

	std::reverse(candles.begin(), candles.end());

	if (figi != m_figi || interval != m_interval) {
		emit sendPrevDayClose(candles.first()->close);
		m_lastCandle = QFCandle(*candles.last());
		m_figi = figi;
		m_interval = interval;
		m_timer->start(1000);
	}

	emit sendCandles(candles);

};


void MonteCarloBroker::brokerChanged()
{
	m_timer->stop();
};
