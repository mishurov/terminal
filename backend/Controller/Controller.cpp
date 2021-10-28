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


#include "Controller.h"

#include "frontend/Blackfriars/Facade.h"
#include "backend/Brokers/MonteCarloBroker.h"
#include "backend/Brokers/TinkoffBroker.h"

#include <QtCore/QSettings>

Controller::Controller(QObject *parent)
	: QObject(parent)
{
	m_facade = new Facade(parent);

	connect(m_facade, &Facade::requestBrokerData, this, &Controller::brokerDataRequested);
	connect(m_facade, &Facade::requestCandles, this, &Controller::candlesRequested);
	connect(m_facade, &Facade::brokerChanged, this, &Controller::brokerChanged);

	m_broker = new MonteCarloBroker(this);
	connectBroker();
	brokers << m_broker;

	m_broker = new TinkoffBroker(this);
	connectBroker();
	brokers << m_broker;

	auto s = facade()->settings();
	const Preferences &p = facade()->preferences();
	m_broker = brokers.at(s->value(p.brokerKey(), p.brokerDefault()).toInt());
}

void Controller::brokerChanged(int broker)
{
	m_broker->brokerChanged();
	m_broker = brokers.at(broker);
	m_broker->requestBrokerData();
}

void Controller::connectBroker()
{
	connect(m_broker, &AbstractBroker::sendBrokerData, m_facade, &Facade::brokerDataRecieved);
	connect(m_broker, &AbstractBroker::sendCandles, m_facade, &Facade::candlesReceived);

	connect(m_broker, &AbstractBroker::sendPrevDayClose, m_facade, &Facade::prevDayCloseReceived);
	connect(m_broker, &AbstractBroker::sendBidAskAverage, m_facade, &Facade::bidAskAverageReceived);

	connect(m_broker, &AbstractBroker::sendLatestCandle, m_facade, &Facade::latestCandleReceived);
}

void Controller::brokerDataRequested()
{
	m_broker->requestBrokerData();
}

void Controller::candlesRequested(QString figi, QDateTime to, int interval, int count)
{
	QDateTime UTC(to.toUTC());
	m_broker->requestCandles(figi, UTC, interval, count);
}

Facade* Controller::facade() { return m_facade; }
