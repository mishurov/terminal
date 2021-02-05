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


#ifndef MONTECARLOBROKER_H
#define MONTECARLOBROKER_H

#include <random>
#include "abstractbroker.h"

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class MonteCarloBroker : public AbstractBroker
{
	Q_OBJECT

public:
	MonteCarloBroker(QObject *parent = nullptr);

	void requestStocks() override;
	void requestCandles(QString figi, int interval) override;

	void brokerChanged() override;
private:
	void genCandle();

	Candle m_lastCandle;
	QTimer *m_timer;
	int m_interval;

	std::mt19937_64 m_engine;
	std::normal_distribution<double> m_gausDist;
	double m_gausMean;
	std::exponential_distribution<double> m_expDist;
};


#endif //MONTECARLOBROKER
