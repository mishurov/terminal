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


#ifndef FACADE_H
#define FACADE_H

#include "backend/Brokers/AbstractBroker.h"

#include <QtCore/QObject>
#include <QtQuick/QQuickItem>
#include <QtQml/qqml.h>


class StockItemModel;
class QSettings;
class QFCandle;


struct Preferences
{
	Q_GADGET
	Q_PROPERTY(QStringList brokers READ brokers)
	Q_PROPERTY(QString brokerKey READ brokerKey)
	Q_PROPERTY(int brokerDefault READ brokerDefault)
	Q_PROPERTY(QString credentialsKey READ credentialsKey)
	Q_PROPERTY(QString riskKey READ riskKey)
	Q_PROPERTY(qreal riskDefault READ riskDefault)
	Q_PROPERTY(QString targetKey READ targetKey)
	Q_PROPERTY(qreal targetDefault READ targetDefault)

public:
	QStringList brokers() const { return { "Monte Carlo", "Tinkoff Investments" }; }
	QString brokerKey() const { return "preferences/broker"; }
	int brokerDefault() const { return 0; }
	QString credentialsKey() const { return "preferences/credentials"; }
	QString riskKey() const { return "preferences/risk"; }
	qreal riskDefault() const { return 2.0; }
	QString targetKey() const { return "preferences/target"; }
	qreal targetDefault() const { return 5.0; }
};


class Facade : public QObject
{
	Q_OBJECT
	Q_PROPERTY(Preferences preferences READ preferences CONSTANT)
	Q_PROPERTY(QList<QFCandle> candles READ candles NOTIFY candlesChanged)
	Q_PROPERTY(QList<int> intervals READ intervals NOTIFY intervalsChanged)
	Q_PROPERTY(qreal prevDayClose READ prevDayClose NOTIFY prevDayCloseChanged)
	Q_PROPERTY(qreal bidAskAverage READ bidAskAverage NOTIFY bidAskAverageChanged)
	QML_ELEMENT
	QML_SINGLETON

signals:
	Q_INVOKABLE void requestBrokerData();
	Q_INVOKABLE void brokerChanged(int brokerIdx);
	Q_INVOKABLE void requestCandles(QString figi, QDateTime to, int interval, int count);
	void candlesChanged();
	void preCandlesChanged();
	void intervalsChanged();
	void prevDayCloseChanged();
	void bidAskAverageChanged();

public:
	Facade(QObject *parent = nullptr);
	Q_INVOKABLE void initWithQmlApi(QObject *qmlApi);
	Q_INVOKABLE void clearBrokerData();
	Q_INVOKABLE void clearAllCandles();
	Q_INVOKABLE void configureWindowTitleBar(const QString &title);

	void brokerDataRecieved(QList<Stock *> stocks, QList<int> intervals);
	void candlesReceived(QList<QFCandle *> candles);
	void prevDayCloseReceived(qreal price);
	void bidAskAverageReceived(qreal price);
	void latestCandleReceived(QFCandle candle);

	const Preferences& preferences() const { return m_preferences; }
	QSettings* settings() const { return m_settings; }
	const QList<QFCandle>& candles() const { return m_candles; }
	const QList<int>& intervals() const { return m_intervals; }
	const qreal prevDayClose() const { return m_prevDayClose; }
	const qreal bidAskAverage() const { return m_bidAskAverage; }

private:
	QObject *m_qmlApi;
	StockItemModel *m_stockModel;
	Preferences m_preferences;
	QSettings *m_settings;
	QList<QFCandle> m_candles;
	QList<int> m_intervals;
	qreal m_prevDayClose;
	qreal m_bidAskAverage;
};

#endif // FACADE_H
