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

#include "frontend/QFCharts/QFCandle.h"

#include <QtCore/QObject>
#include <QtCore/QTime>


struct Stock {
	QString ticker;
	QString name;
	QString figi;
};

class Controller;

class AbstractBroker : public QObject
{
	Q_OBJECT

signals:
	void sendBrokerData(QList<Stock *>, QList<int> intervals);
	void sendCandles(QList<QFCandle *> candles);
	void sendPrevDayClose(qreal price);
	void sendBidAskAverage(qreal price);
	void sendLatestCandle(QFCandle candle);

public:
	AbstractBroker(Controller *parent = nullptr);

	virtual void requestBrokerData() = 0;
	virtual void requestCandles(QString figi, QDateTime to, int interval, int count) = 0;

	virtual void brokerChanged() {};
	Controller* parent() const;

};


#endif //ABSTRACTBROKER
