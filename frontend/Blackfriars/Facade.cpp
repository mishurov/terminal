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


#include "Facade.h"

#include "StockItemModel.h"
#include "frontend/QFCharts/QFCandle.h"

#if defined(Q_OS_MACOS)
#include "TitleBarMacOS.h"
#include <QtGui/QWindow>
#elif defined(Q_OS_WIN)
#include "TitleBarWindows.h"
#endif

#include <QtCore/QSettings>
#include <QtWidgets/QApplication>


Facade::Facade(QObject *parent)
	: QObject(parent),
	m_prevDayClose(0),
	m_bidAskAverage(0)
{
#if defined(Q_OS_MACOS)
	m_settings = new QSettings(QApplication::organizationDomain(),
		QApplication::applicationName());
#else
	m_settings = new QSettings(QApplication::organizationName(),
		QApplication::applicationName());
#endif
}

void Facade::initWithQmlApi(QObject *qmlApi)
{
	m_qmlApi = qmlApi;
	m_stockModel = m_qmlApi->findChild<StockItemModel *>();
}

void Facade::configureWindowTitleBar(const QString &title) {
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
	foreach (QWindow *window, QApplication::allWindows()) {
		if (title == window->title()) {
			configureTitleBar(window->winId());
			break;
		}
	}
#endif
}

void Facade::clearAllCandles()
{
	m_candles.clear();
	emit candlesChanged();

	m_prevDayClose = 0;
	emit prevDayCloseChanged();

	m_bidAskAverage = 0;
	emit bidAskAverageChanged();
}


void Facade::clearBrokerData()
{
	m_stockModel->clear();
	m_intervals.clear();
	emit intervalsChanged();
}

void Facade::brokerDataRecieved(QList<Stock *> stocks, QList<int> intervals)
{
	if (stocks.empty())
		return;

	m_stockModel->setStocks(stocks);
	m_intervals = intervals;

	while (!stocks.isEmpty())
		delete stocks.takeFirst();

	emit intervalsChanged();
}


void Facade::candlesReceived(QList<QFCandle *> candles)
{
	if (candles.empty())
		return;
	if (!m_candles.size()) {
		foreach(QFCandle *c, candles)
			m_candles.push_back(*c);
	} else {
		for (int i = candles.size() - 1; i >= 0; i--)
			m_candles.push_front(*candles.at(i));
	}

	while (!candles.isEmpty())
		delete candles.takeFirst();

	emit candlesChanged();
}


void Facade::prevDayCloseReceived(qreal price)
{
	m_prevDayClose = price;
	emit prevDayCloseChanged();
}


void Facade::bidAskAverageReceived(qreal price)
{
	m_bidAskAverage = price;
	emit bidAskAverageChanged();
}


void Facade::latestCandleReceived(QFCandle candle)
{
	if (m_candles.last().time < candle.time)
		m_candles.push_back(candle);
	else
		m_candles.last() = candle;

	emit candlesChanged();
}
