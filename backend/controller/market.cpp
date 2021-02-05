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


#include "market.h"
#include "frontend/facade/market.h"
#include "backend/controller/preferences.h"
#include "backend/brokers/montecarlobroker.h"
#include "backend/brokers/tinkoffbroker.h"

#include <QtCore/QDebug>

MarketController::MarketController(QObject *parent)
	: QObject(parent)
{
	m_facade = new MarketFacade(parent);

	connect(m_facade, &MarketFacade::requestStocks, this, &MarketController::stocksRequested);
	connect(m_facade, &MarketFacade::requestCandles, this, &MarketController::candlesRequested);

	m_broker = new MonteCarloBroker(parent);
	connectBroker();
	brokers << m_broker;

	m_broker = new TinkoffBroker(parent);
	connectBroker();
	brokers << m_broker;

	m_broker = brokers.at(preferences.read()->broker);
	connect(&preferences, &Preferences::brokerChanged, this, &MarketController::brokerChanged);
}

void MarketController::brokerChanged(int broker)
{
	m_broker->brokerChanged();
	m_broker = brokers.at(broker);
	m_broker->requestStocks();
}

void MarketController::connectBroker()
{
	connect(m_broker, &AbstractBroker::sendStocks, m_facade, &MarketFacade::stocksRecieved);
	connect(m_broker, &AbstractBroker::sendCandles, m_facade, &MarketFacade::candlesReceived);
	connect(m_broker, &AbstractBroker::sendExchangeHours, m_facade, &MarketFacade::exchangeHoursReceived);
	connect(m_broker, &AbstractBroker::sendPrevClose, m_facade, &MarketFacade::prevCloseReceived);

	connect(m_broker, &AbstractBroker::sendLatestCandle, m_facade, &MarketFacade::latestCandleReceived);
	connect(m_broker, &AbstractBroker::sendBidAskPrice, m_facade, &MarketFacade::bidAskPriceReceived);
}

void MarketController::stocksRequested()
{
	m_broker->requestStocks();
}

void MarketController::candlesRequested(QString figi, int interval)
{
	m_broker->requestCandles(figi, interval);
}

MarketFacade* MarketController::facade() { return m_facade; }
