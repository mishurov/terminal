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


#ifndef ABSTRACTBROKER_H
#define ABSTRACTBROKER_H

#include <QtCore/QObject>
#include <QtCore/QTime>


struct Candle {
	double open;
	double close;
	double high;
	double low;
	int volume;
	QDateTime time;
};


struct Stock {
	QString ticker;
	QString name;
	QString figi;
};


class AbstractBroker : public QObject
{
	Q_OBJECT

public:
	AbstractBroker(QObject *parent = nullptr);

	virtual void requestStocks() = 0;
	virtual void requestCandles(QString figi, int interval) = 0;
	
	virtual void brokerChanged() {};
signals:
	void sendStocks(QList<Stock *>);
	void sendCandles(QList<Candle *> candles, int interval);
	void sendExchangeHours(QTime open, QTime close);
	void sendPrevClose(double price);
	void sendLatestCandle(Candle candle);
	void sendBidAskPrice(double price);
};


#endif //ABSTRACTBROKER
