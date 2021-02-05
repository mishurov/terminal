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


#ifndef MARKETFACADE_H
#define MARKETFACADE_H

#include <QtCore/QObject>
#include <QtWidgets/QButtonGroup>
#include "backend/brokers/abstractbroker.h"

class TickerInput;
class ChartPane;
class RightPane;
class FinChartView;


class MarketFacade : public QObject
{
	Q_OBJECT
	// TODO filter candlestick mouse over listening
public:
	MarketFacade(QObject *parent = nullptr);

signals:
	void requestStocks();
	void requestCandles(QString figi, int interval);

public:
	void onEventLoopStarted();
	void stocksRecieved(QList<Stock *> stocks);

	void exchangeHoursReceived(QTime open, QTime close);
	void candlesReceived(QList<Candle *> candles, int interval);
	void prevCloseReceived(double price);

	void latestCandleReceived(Candle candle);
	void bidAskPriceReceived(double price);

private:
	void candlesRequested();

	TickerInput* m_tickerInput;
	FinChartView* m_finChartView;
	ChartPane* m_chartPane;
	RightPane* m_rightPane;
};

#endif //MARKETFACADE_H
