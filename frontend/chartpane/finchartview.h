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


#ifndef FINCHARTVIEW_H
#define FINCHARTVIEW_H

#include "backend/brokers/abstractbroker.h"

#include <QtCharts/QChartView>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>

class QCandlestickSeries;
class QStackedBarSeries;
class QCandlestickSet;
class QValueAxis;

class CrossHair;
class Shade;
class FinTimeAxis;


inline qint64 timeToQint(QDateTime time) {
	return time.toMSecsSinceEpoch();
}

inline QDateTime quintToTime(qint64 msecs) {
	return QDateTime::fromMSecsSinceEpoch(msecs);
}

class FinChart : public QChart
{
	Q_OBJECT

	Q_PROPERTY(qreal axesFontSize READ axesFontSize WRITE setAxesFontSize)

public:
	FinChart(QGraphicsItem *parent=nullptr,
		Qt::WindowFlags wFlags=Qt::WindowFlags());

	QValueAxis* priceAxis();
	QValueAxis* volumeAxis();
	FinTimeAxis* timeAxis();

	QCandlestickSeries* priceSeries();
	QStackedBarSeries* volumeSeries();

	QDateTime startTime();
	qint64 unitSecSpan();

	void setCandles(const QList<Candle *> &candles);
	void appendCandle(Candle *candle);
	void editCandle(Candle *candle);

	qreal axesFontSize();
	void setAxesFontSize(qreal size);

	QPair<qreal, qreal> visibleTimeRange();

	void setExchangeHours(QTime open, QTime close);

private:
	void configureAxes();
	void updateVisibleRanges();
	void applyStyles();

	int computeMinMax();

	void rangeChanged(const QString &min, const QString &max);
	void plotAreaChanged(const QRectF &plotArea);

	void adjustPriceTicks(const QRectF &plotArea);

	void updateShades();

	using QChart::wheelEvent;
	void wheelEvent(QWheelEvent *event);
	using QChart::mousePressEvent;
	void mousePressEvent(QMouseEvent *event);
	using QChart::mouseMoveEvent;
	void mouseMoveEvent(QMouseEvent *event);

	QPointF m_lastMousePos;

	Shade* m_preMarketShade = nullptr;
	Shade* m_postMarketShade = nullptr;

	QValueAxis* m_priceAxis;
	QValueAxis* m_volumeAxis;
	FinTimeAxis* m_timeAxis;

	QCandlestickSeries* m_priceSeries;
	QStackedBarSeries* m_volumeSeries;

	qreal m_maxVisiblePrice;
	qreal m_minVisiblePrice;
	qreal m_maxVisibleVolume;

	QDateTime m_startTime;
	QDateTime m_endTime;
	
	qint64 m_seriesSecSpan;
	qint64 m_unitSecSpan;

	qreal m_volumeHeight = 0.25;
	qreal m_priceTopBorder = 0.025;
	qreal m_priceBottomBorder = 0.135;
	qreal m_minTickHeight = 27;
	qreal m_minTickWidth = 54;

	int m_resetOffsetSticks = 10;
	int m_resetZoomSticks = 100;
	int m_maxZoomSticks = 1000;
	int m_minZoomSticks = 10;

	QTime m_exchangeOpen;
	QTime m_exchangeClose;

	friend class FinChartView;
};

class FinChartView : public QChartView
{
	Q_OBJECT

public:
	FinChartView(QWidget *parent=nullptr);

	FinChart* chart();
	void setChart(FinChart *chart);

	void setCandles(const QList<Candle *> &candles, int interval);
	void setLatestCandle(Candle candle);
	CrossHair* stopLoss();
	CrossHair* takeProfit();
protected:
	void wheelEvent(QWheelEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
private:
	void updatePriceLine();

	FinChart* m_chart;

	CrossHair* m_crossHairTime;
	CrossHair* m_crossHairPrice;
	CrossHair* m_priceLine;
	CrossHair* m_stopLoss;
	CrossHair* m_takeProfit;

	Candle m_latestCandle;
};

void applyStyle(QObject *obj, const QVariantMap *style);

#endif // FINCHARTVIEW_H
