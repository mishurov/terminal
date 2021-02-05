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


#include "finchartview.h"
#include "finchartview_ss.h"

#include "crosshair.h"

#include "fintimeaxis.h"

#include "backend/controller/market.h"

#include <QtMath>
#include <QtAlgorithms>
#include <QGraphicsLayout>
#include <QtCharts/QCandlestickSet>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QGraphicsSceneWheelEvent>

#include <QTimeZone>

#include <QDebug>


void applyStyle(QObject *obj, const QVariantMap *style)
{
	for(auto s : style->keys())
		obj->setProperty(s.toLatin1().data(), style->value(s));
}


FinChart::FinChart(QGraphicsItem *parent, Qt::WindowFlags wFlags)
	:QChart(parent, wFlags)
{
	connect(this, &QChart::plotAreaChanged, this, &FinChart::plotAreaChanged);

	m_priceSeries = new QCandlestickSeries;
	m_volumeSeries = new QCandlestickSeries;

	addSeries(m_volumeSeries);
	addSeries(m_priceSeries);

	configureAxes();

	setMargins(QMargins());
	layout()->setContentsMargins(0, 0, 5, 0);
	legend()->setVisible(false);

	setTheme(QChart::ChartThemeDark);
	applyStyles();

	updateTimeData();
}

void FinChart::configureAxes()
{
	m_timeAxis = new QDateTimeAxis(this);
	//m_timeAxis = new FinTimeAxis(this);
	//auto axis = (FinTimeAxis *)m_timeAxis;
	addAxis(m_timeAxis, Qt::AlignBottom);
	m_priceSeries->attachAxis(m_timeAxis);
	m_volumeSeries->attachAxis(m_timeAxis);
	m_timeAxis->setFormat("HH:mm");

	m_priceAxis = new QValueAxis(this);
	addAxis(m_priceAxis, Qt::AlignRight);
	m_priceSeries->attachAxis(m_priceAxis);
	m_priceAxis->setLabelFormat("%.2f");
	m_priceAxis->setGridLineVisible(true);

	m_volumeAxis = new QValueAxis(this);
	addAxis(m_volumeAxis, Qt::AlignLeft);
	m_volumeSeries->attachAxis(m_volumeAxis);
	m_volumeAxis->setVisible(false);

	connect(m_timeAxis, &QDateTimeAxis::rangeChanged, this, &FinChart::rangeChanged);
}

void FinChart::updateTimeData()
{
	auto sets = m_priceSeries->sets();

	if (!sets.count()) {
		m_priceAxis->setTickType(QValueAxis::TicksFixed);
		m_priceAxis->setRange(0, 100);
		m_volumeAxis->setRange(0, 100);
		auto now = QDateTime::currentDateTime();
		m_timeAxis->setRange(now.addDays(-1), now);
		return;
	}

	auto first = sets.first()->timestamp();
	auto second = sets.at(1)->timestamp();
	auto third = sets.at(2)->timestamp();
	auto last = sets.last()->timestamp();

	m_startTime = quintToTime(first);
	m_endTime = quintToTime(last);
	auto secondTime = quintToTime(second);
	auto thirdTime = quintToTime(third);

	auto firstSpan = m_startTime.secsTo(secondTime);
	auto secondSpan = secondTime.secsTo(thirdTime);

	m_seriesSecSpan = m_startTime.secsTo(m_endTime);
	m_unitSecSpan = qMin(firstSpan, secondSpan);

	auto backSecs = - (m_resetZoomSticks - m_resetOffsetSticks) *  m_unitSecSpan;
	auto frontSecs = m_resetOffsetSticks *  m_unitSecSpan;
	m_timeAxis->setRange(m_endTime.addSecs(backSecs), m_endTime.addSecs(frontSecs));
}

void FinChart::setCandles(const QList<Candle *> &candles)
{
	m_priceAxis->setTickType(QValueAxis::TicksFixed);
	m_priceSeries->clear();
	m_volumeSeries->clear();
	
	QPen dec(m_priceSeries->decreasingColor());
	QPen inc(m_priceSeries->increasingColor());

	QList<QCandlestickSet *> priceSets;
	QList<QCandlestickSet *> volumeSets;
	
	auto utcOffset = QDateTime::currentDateTime().offsetFromUtc() * 1000;
	for (auto c : candles) {
		auto ts = c->time.toMSecsSinceEpoch();
		ts -= utcOffset;
		auto p = new QCandlestickSet(
			c->open,
			c->high,
			c->low,
			c->close,
			ts
		);
		qreal open = 0, close = 0;
		if (c->close > c->open) {
			p->setPen(inc);
			close = c->volume;
		} else {
			p->setPen(dec);
			open = c->volume;
		}
		auto v = new QCandlestickSet(open, close, open, close, ts);
		priceSets.append(p);
		volumeSets.append(v);
	}

	m_priceSeries->append(priceSets);
	m_volumeSeries->append(volumeSets);

	updateTimeData();
}

void FinChart::appendCandle(Candle *candle)
{
	auto min = m_timeAxis->min();
	auto max = m_timeAxis->max();

	// TODO make as an API function
	// TODO BRUSH / PEN from reader
	auto ts = QString("1438387200000").toDouble();
	m_priceSeries->append(new QCandlestickSet(122.60, 122.64, 120.91, 121.30, ts));
	m_volumeSeries->append(new QCandlestickSet(0, 0, 0, 0, ts));
	// end

	if (m_endTime <= max)
		m_timeAxis->setRange(min.addSecs(m_unitSecSpan), max.addSecs(m_unitSecSpan));
	else
		m_timeAxis->setRange(min, max);
}

void FinChart::editCandle(Candle *candle)
{
	auto min = m_timeAxis->min();
	auto max = m_timeAxis->max();

	// TODO make as an API function
	auto can = m_priceSeries->sets().last();
	can->setClose(122.60);
	can->setHigh(122.64);
	can->setLow(120.91);
	can->setOpen(122.10);
	// end

	m_timeAxis->setRange(min, max);
}

void FinChart::plotAreaChanged(const QRectF &plotArea)
{
	adjustPriceTicks(plotArea);
	adjustTimeTicks(plotArea);
}

void FinChart::rangeChanged(QDateTime min, QDateTime max)
{
	if (computeMinMax(min, max) < 0)
		return;

	m_volumeAxis->setRange(0, m_maxVisibleVolume * (1 / m_volumeHeight));

	auto priceRange = m_maxVisiblePrice - m_minVisiblePrice;
	auto topBorder = priceRange * m_priceTopBorder;
	auto bottomBorder = priceRange * m_priceBottomBorder;
	m_priceAxis->setRange(m_minVisiblePrice - bottomBorder,
		m_maxVisiblePrice + topBorder);

	adjustPriceTicks(plotArea());
}

void FinChart::adjustPriceTicks(const QRectF &plotArea)
{
	if (!m_priceSeries->sets().count())
		return;

	auto pHeight = plotArea.height();
	if (pHeight < m_minTickHeight)
		return;

	auto priceRange = m_maxVisiblePrice - m_minVisiblePrice;

	int maxTickNum = pHeight / m_minTickHeight;
	qreal minPriceInterval = priceRange *
		(1 + m_priceTopBorder + m_priceBottomBorder) / maxTickNum;
	
	qreal decimalMultiplier = std::pow(10, std::floor(std::log10(minPriceInterval)));
	qreal normInterval = minPriceInterval / decimalMultiplier;
	qreal intervals[4] = {1, 2, 5, 10};

	qreal closest;
	qreal minDiff = 999999;
	for (int i = 0; i < 4; i++) {
		if (intervals[i] < normInterval)
			continue;
		qreal absDiff = qAbs(normInterval - intervals[i]);
		if (absDiff <= minDiff) {
			minDiff = absDiff;
			closest = intervals[i];
		}
	}

	qreal tickInterval = closest * decimalMultiplier;

	m_priceAxis->setTickType(QValueAxis::TicksDynamic);
	m_priceAxis->setTickAnchor(0);
	m_priceAxis->setTickInterval(tickInterval);
}

void FinChart::adjustTimeTicks(const QRectF &plotArea)
{
	auto pWidth = plotArea.width();
	if (pWidth < m_minTickWidth)
		return;

	int	numTicks = qFloor(pWidth / m_minTickWidth) - 1;

	m_timeAxis->setTickCount(numTicks);
}

int FinChart::computeMinMax(QDateTime &min, QDateTime &max)
{
	int i = boundIndex(min, max);

	if (i < 0)
		return -1;
	
	auto priceSets = m_priceSeries->sets();
	auto volumeSets = m_volumeSeries->sets();

	m_maxVisibleVolume = 0;
	m_maxVisiblePrice = 0;
	m_minVisiblePrice = 99999999;

	QDateTime ts;
	bool isReverse = i >= priceSets.count() - 1;

	while (i >= 0 && i < priceSets.count()) {
		auto *pSet = priceSets.at(i);
		auto *vSet = volumeSets.at(i);
		ts = quintToTime(pSet->timestamp());
		if (ts > max || ts < min)
			break;
		m_maxVisiblePrice = qMax(pSet->high(), m_maxVisiblePrice);
		m_minVisiblePrice = qMin(pSet->low(), m_minVisiblePrice);
		qreal volume = vSet->open() > 0 ? vSet->open() : vSet->close();
		m_maxVisibleVolume = qMax(volume, m_maxVisibleVolume);

		isReverse ? i-- : i++;
	}

	return 0;
}

int FinChart::boundIndex(QDateTime &min, QDateTime &max)
{
	// if bounds of the whole series visible, returns corresponding bound idx
	auto sets = m_priceSeries->sets();
	if (!sets.count())
		return -1;

	if (m_startTime >= min)
		return 0;
	if (m_endTime <= max)
		return sets.count() - 1;

	// binary search for nearest to min visible candlestick
	auto left = [] (QCandlestickSet *set, QDateTime value) {
		return quintToTime(set->timestamp()) < value;
	};

	QList<QCandlestickSet *>::iterator it = std::lower_bound(
		sets.begin(), sets.end(), min, left);

	if (it == sets.end())
		return -1;

	return it - sets.begin();
}

void FinChart::wheelEvent(QWheelEvent *event)
{
	if (!m_priceSeries->sets().count())
		return;

	const qreal factor = 1.001;
	QRectF r = plotArea();

	qreal right = r.bottomRight().x();
	qreal val = qPow(factor, -event->angleDelta().y());
	r.setWidth(r.width() * val);

	auto br = mapToValue(r.bottomRight(), m_priceSeries);
	auto tl = mapToValue(r.topLeft(), m_priceSeries);
	auto rightDate = quintToTime(br.x());
	auto leftDate = quintToTime(tl.x());

	auto plotSpan = leftDate.secsTo(rightDate);
	auto minSpan = m_unitSecSpan * m_minZoomSticks;
	auto maxSpan = m_unitSecSpan * m_maxNumSticks;

	if ((val > 1 && plotSpan < maxSpan) || (val < 1 && plotSpan > minSpan)) {
		r.moveRight(right);
		zoomIn(r);
	}
}

void FinChart::mousePressEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton || !m_priceSeries->sets().count())
		return;
	m_lastMousePos = event->pos();
	event->accept();
}

void FinChart::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton) || !m_priceSeries->sets().count())
		return;

	auto dPos = event->pos() - m_lastMousePos;
	int dPosX = -dPos.x();
	
	auto plot = plotArea();
	auto br = mapToValue(plot.bottomRight(), m_priceSeries);
	auto tl = mapToValue(plot.topLeft(), m_priceSeries);
	auto rightDate = quintToTime(br.x());
	auto leftDate = quintToTime(tl.x());

	if (leftDate > m_endTime && dPosX > 0) {
		dPosX = 0;
	}

	if (rightDate < m_startTime && dPosX < 0) {
		dPosX = 0;
	}

	scroll(dPosX, 0);

	m_lastMousePos = event->pos();

	event->accept();
}

void FinChart::applyStyles()
{
	applyStyle(this, &chartStyle);
	applyStyle(m_priceSeries, &priceStyle);
	applyStyle(m_volumeSeries, &volumeStyle);
	applyStyle(m_priceAxis, &axisStyle);
	applyStyle(m_timeAxis, &axisStyle);
}

qreal FinChart::axesFontSize()
{
	return m_priceAxis->labelsFont().pointSizeF();
}

void FinChart::setAxesFontSize(qreal size) {
	auto font = m_priceAxis->labelsFont();
	font.setPointSizeF(size);
	m_priceAxis->setLabelsFont(font);
	m_timeAxis->setLabelsFont(font);
}

QValueAxis* FinChart::priceAxis() { return m_priceAxis; }

QValueAxis* FinChart::volumeAxis() { return m_volumeAxis; }

QDateTimeAxis* FinChart::timeAxis() { return m_timeAxis; }

QCandlestickSeries* FinChart::priceSeries() { return m_priceSeries; }

QCandlestickSeries* FinChart::volumeSeries() { return m_volumeSeries; }

QDateTime FinChart::startTime() { return m_startTime; }

qint64 FinChart::unitSecSpan() { return m_unitSecSpan; }



FinChartView::FinChartView(QWidget *parent)
	: QChartView(parent)
{
	setChart(new FinChart);
	setMouseTracking(true);

	m_crossHairTime = new CrossHair(m_chart, Qt::Vertical);
	m_crossHairTime->setDraggable(false);
	m_crossHairTime->setFollowRange(false);
	m_crossHairTime->setZValue(4);
	applyStyle(m_crossHairTime, &crossHairStyle);

	m_priceLine = new CrossHair(m_chart, Qt::Horizontal);
	m_priceLine->setDraggable(false);
	m_priceLine->setZValue(4);
	applyStyle(m_priceLine, &priceLineStyle);
	m_priceLine->setVisible(false);
}

void FinChartView::updatePriceLine()
{
	auto last = m_chart->priceSeries()->sets().last();
	bool isUp = last->close() >= last->open();
	m_priceLine->setIsUp(isUp);
	m_priceLine->setValue(QPointF(1, last->close()));
}

void FinChartView::setCandles(const QList<Candle *> &candles) {
	m_chart->setCandles(candles);
	updatePriceLine();
	m_priceLine->setVisible(true);
}

void FinChartView::appendCandle(Candle *candle)
{
	m_chart->appendCandle(candle);
	updatePriceLine();
}

void FinChartView::editCandle(Candle *candle) {
	m_chart->editCandle(candle);
	updatePriceLine();
}

void FinChartView::setChart(FinChart *chart)
{
	m_chart = chart;
	QChartView::setChart(chart);
}

void FinChartView::mouseMoveEvent(QMouseEvent *event)
{
	m_chart->mouseMoveEvent(event);

	m_crossHairTime->setPos(event->pos());

	QChartView::mouseMoveEvent(event);
}

//static int test = 0;

void FinChartView::mousePressEvent(QMouseEvent *event)
{
	// TODO for test
	/*
	if (event->button() == Qt::RightButton) {
		if (test == 0) {
			QList<Candle *> c;
			setCandles(c);
			test = 1;
		} else if (test == 1) {
			appendCandle(nullptr);
			test = 2;
		} else if (test == 2) {
			editCandle(nullptr);
			test = 3;
		}
	}
	*/
	m_chart->mousePressEvent(event);
	QChartView::mousePressEvent(event);
}

void FinChartView::wheelEvent(QWheelEvent *event)
{
	m_chart->wheelEvent(event);
	QChartView::wheelEvent(event);
}
