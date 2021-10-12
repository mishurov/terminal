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

#include <QtCharts/QChartView>
#include <QDateTime>
#include <QDebug>

class QCandlestickSeries;
class QCandlestickSet;
class QDateTimeAxis;
class QValueAxis;

class CrossHair;
class Candle;

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
	QDateTimeAxis* timeAxis();

	QCandlestickSeries* priceSeries();
	QCandlestickSeries* volumeSeries();

	QDateTime startTime();
	qint64 unitSecSpan();

	void setCandles(const QList<Candle *> &candles);
	void appendCandle(Candle *candle);
	void editCandle(Candle *candle);

	qreal axesFontSize();
	void setAxesFontSize(qreal size);

private:
	void configureAxes();
	void updateTimeData();
	void applyStyles();

	int boundIndex(QDateTime &min, QDateTime &max);
	int computeMinMax(QDateTime &min, QDateTime &max);

	void rangeChanged(QDateTime min, QDateTime max);
	void plotAreaChanged(const QRectF &plotArea);

	void adjustPriceTicks(const QRectF &plotArea);
	void adjustTimeTicks(const QRectF &plotArea);

	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	QPointF m_lastMousePos;

	QValueAxis* m_priceAxis;
	QValueAxis* m_volumeAxis;
	QDateTimeAxis* m_timeAxis;

	QCandlestickSeries* m_priceSeries;
	QCandlestickSeries* m_volumeSeries;

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

	int m_resetZoomSticks = 100;
	int m_resetOffsetSticks = 10;
	int m_minZoomSticks = 10;
	int m_maxNumSticks = 1000;

	friend class FinChartView;
};

class FinChartView : public QChartView
{
	Q_OBJECT

public:
	FinChartView(QWidget *parent=nullptr);

	FinChart* chart();
	void setChart(FinChart *chart);

	void setCandles(const QList<Candle *> &candles);
	void appendCandle(Candle *candle);
	void editCandle(Candle *candle);
protected:
	void wheelEvent(QWheelEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
private:
	void updatePriceLine();

	FinChart *m_chart;

	CrossHair *m_crossHairTime;
	CrossHair *m_priceLine;
};

void applyStyle(QObject *obj, const QVariantMap *style);

#endif // FINCHARTVIEW_H
