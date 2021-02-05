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


#include "frontend/facade/market.h"

#include "frontend/rightpane/rightpane.h"
#include "frontend/chartpane/chartpane.h"
#include "frontend/chartpane/finchartview.h"
#include "frontend/tickerinput/tickerinput.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>

#include <QtCore/QDebug>


MarketFacade::MarketFacade(QObject *parent)
	: QObject(parent)
{
	const QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
	QWidget* mw;
	for (auto w : topLevelWidgets) {
		if (w->objectName() == "mainWindow") {
			mw = w;
			break;
		}
	}

	m_chartPane = mw->findChild<ChartPane *>("chartPane");
	m_rightPane = mw->findChild<RightPane *>("rightPane");
	m_tickerInput = mw->findChild<TickerInput *>("tickerInput");
	m_finChartView = mw->findChild<FinChartView *>("finChartView");

	connect(m_tickerInput, &TickerInput::confirmed, this, &MarketFacade::candlesRequested);
	connect(m_chartPane, &ChartPane::intervalSet, this, &MarketFacade::candlesRequested);
}


void MarketFacade::candlesRequested()
{
	QString figi = m_tickerInput->figi();
	int interval = m_chartPane->interval();
	// TODO handle network errors etc?
	if (figi.isEmpty() || interval < 1)
		return;

	emit requestCandles(figi, interval);
}

void MarketFacade::onEventLoopStarted()
{
	emit requestStocks();
}

void MarketFacade::stocksRecieved(QList<Stock *> stocks)
{
	if (stocks.empty())
		return;

	m_tickerInput->setModel(stocks);
}

void MarketFacade::candlesReceived(QList<Candle *> candles, int interval)
{
	if (candles.empty())
		return;

	m_finChartView->setCandles(candles, interval);

	m_rightPane->setCurrent(candles.last()->close);
}

void MarketFacade::exchangeHoursReceived(QTime open, QTime close)
{
	m_finChartView->chart()->setExchangeHours(open, close);
}

void MarketFacade::prevCloseReceived(double price)
{
	m_rightPane->setPrevClose(price);
}

void MarketFacade::latestCandleReceived(Candle candle)
{
	m_finChartView->setLatestCandle(candle);
}

void MarketFacade::bidAskPriceReceived(double price)
{
	m_rightPane->setCurrent(price);
}
