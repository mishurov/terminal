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
#include "shade.h"

#include "fintimeaxis.h"

#include <QtCore/QtMath>
#include <QtCore/QtAlgorithms>
#include <QtWidgets/QGraphicsLayout>

#include <QtCharts/QCandlestickSet>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QGraphicsSceneWheelEvent>

#include <QtCore/QTimeZone>

#include <QtCore/QDebug>


void applyStyle(QObject *obj, const QVariantMap *style)
{
	for(auto s : style->keys())
		obj->setProperty(s.toLatin1().data(), style->value(s));
}


FinChart::FinChart(QGraphicsItem *parent, Qt::WindowFlags wFlags)
	:QChart(parent, wFlags)
{
	connect(this, &QChart::plotAreaChanged, this, &FinChart::plotAreaChanged);

	setAcceptTouchEvents(false);

	m_priceSeries = new QCandlestickSeries;
	m_volumeSeries = new QStackedBarSeries;

	addSeries(m_volumeSeries);
	addSeries(m_priceSeries);

	configureAxes();

	setMargins(QMargins());
	layout()->setContentsMargins(0, 0, 5, 0);
	legend()->setVisible(false);

	setTheme(QChart::ChartThemeDark);


	m_preMarketShade = new Shade(this);
	m_postMarketShade = new Shade(this);

	applyStyles();

	updateVisibleRanges();
}

void FinChart::configureAxes()
{
	m_timeAxis = new FinTimeAxis(this);
	addAxis(m_timeAxis, Qt::AlignBottom);

	m_priceSeries->attachAxis(m_timeAxis);
	m_volumeSeries->attachAxis(m_timeAxis);

	m_priceAxis = new QValueAxis(this);
	addAxis(m_priceAxis, Qt::AlignRight);
	m_priceSeries->attachAxis(m_priceAxis);
	m_priceAxis->setLabelFormat("%.2f");
	m_priceAxis->setGridLineVisible(true);

	m_volumeAxis = new QValueAxis(this);
	addAxis(m_volumeAxis, Qt::AlignLeft);
	m_volumeSeries->attachAxis(m_volumeAxis);
	m_volumeAxis->setVisible(false);

	connect(m_timeAxis, &QBarCategoryAxis::rangeChanged, this, &FinChart::rangeChanged);
}

void FinChart::updateVisibleRanges()
{
	auto sets = m_priceSeries->sets();

	if (!sets.count()) {
		m_priceAxis->setTickType(QValueAxis::TicksFixed);
		m_priceAxis->setRange(0, 100);
		m_volumeAxis->setRange(0, 100);
		return;
	}

	QPair<qreal, qreal> range = visibleTimeRange();

	auto unitVisWidth = plotArea().width() / (range.second - range.first);
	auto unitResWidth = unitVisWidth;

	if (sets.count() > m_resetZoomSticks) {
		unitResWidth = plotArea().width() / m_resetZoomSticks;
		QRectF r = plotArea();
		r.setWidth(r.width() * unitVisWidth / unitResWidth);
		r.moveRight(plotArea().right());
		zoomIn(r);
	}

	scroll(unitResWidth * m_resetOffsetSticks, 0);
}

void FinChart::setExchangeHours(QTime open, QTime close)
{
	m_exchangeOpen = open;
	m_exchangeClose = close;

	updateShades();
}

void FinChart::setCandles(const QList<Candle *> &candles)
{
	m_priceAxis->setTickType(QValueAxis::TicksFixed);
	m_timeAxis->clear();
	m_timeAxis->clearDayIndices();
	m_priceSeries->clear();
	m_volumeSeries->clear();
	
	QPen decPen(m_priceSeries->decreasingColor());
	QPen incPen(m_priceSeries->increasingColor());

	QList<QCandlestickSet *> priceSets;
	
	auto incVolumes = new QBarSet("increasing");
	auto decVolumes = new QBarSet("decreasing");

	incVolumes->setBrush(m_volumeSeries->property("increasingColor").value<QColor>());
	decVolumes->setBrush(m_volumeSeries->property("decreasingColor").value<QColor>());
	incVolumes->setPen(Qt::NoPen);
	decVolumes->setPen(Qt::NoPen);
	
	QStringList categories;

	QDateTime prevDate;

	int i = 0;
	for (auto c : candles) {
		auto ts = c->time.toMSecsSinceEpoch();
		auto p = new QCandlestickSet(
			c->open,
			c->high,
			c->low,
			c->close,
			ts
		);
		qreal decVol = 0, incVol = 0;
		if (c->close > c->open) {
			p->setPen(incPen);
			incVol = c->volume;
		} else {
			p->setPen(decPen);
			decVol = c->volume;
		}
		
		auto cat = c->time.toString(Qt::ISODate);
		*incVolumes << incVol;
		*decVolumes << decVol;
		priceSets.append(p);

		categories << cat;

		if (prevDate.isValid() && prevDate.date().day() != c->time.date().day())
			m_timeAxis->appendDayIndex(i);

		prevDate = c->time;
		i++;
	}

	m_timeAxis->setCategories(categories);
	m_priceSeries->append(priceSets);
	m_volumeSeries->append({ incVolumes, decVolumes });

	updateVisibleRanges();
}


void FinChart::appendCandle(Candle *candle)
{
	QPen decPen(m_priceSeries->decreasingColor());
	QPen incPen(m_priceSeries->increasingColor());

	auto timeStamp = candle->time.toMSecsSinceEpoch();
	auto set = new QCandlestickSet(
		candle->open,
		candle->high,
		candle->low,
		candle->close,
		timeStamp
	);

	qreal decVol = 0, incVol = 0;

	if (candle->close > candle->open) {
		set->setPen(incPen);
		incVol = candle->volume;
	} else {
		set->setPen(decPen);
		decVol = candle->volume;
	}
	
	// save range before the axis resets it for the new category
	QPair<qreal, qreal> range = visibleTimeRange();
	
	auto cat = candle->time.toString(Qt::ISODate);
	m_timeAxis->append(cat);

	m_priceSeries->append(set);

	auto volumeSets = m_volumeSeries->barSets();
	auto *incSet = volumeSets.at(0);
	auto *decSet = volumeSets.at(1);
	incSet->append(incVol);
	decSet->append(decVol);

	// move right one unit
	range.first++;
	range.second++;

	m_timeAxis->setRangeF(range.first, range.second);

	updateShades();
}


void FinChart::updateShades()
{
	auto sets = m_priceSeries->sets();
	QDateTime latest = quintToTime(sets.last()->timestamp()).toUTC();

	QDateTime day = latest;
	day.setTime(QTime::fromString("00:00","hh:mm"));

	QDateTime min, max;

	min = day;
	max = day;
	max.setTime(m_exchangeOpen);
	m_preMarketShade->setTimeRange(min, max);

	if (latest.time() < m_exchangeClose) {
		max = day;
		min = max.addDays(-1);
		min.setTime(m_exchangeClose);
		m_postMarketShade->setTimeRange(min, max);
	} else {
		min = latest;
		min.setTime(m_exchangeClose);
		max = day.addDays(1);
		m_postMarketShade->setTimeRange(min, max);
	}
}

void FinChart::editCandle(Candle *candle)
{
	auto c = m_priceSeries->sets().last();
	c->setOpen(candle->open);
	c->setHigh(candle->high);
	c->setLow(candle->low);
	c->setClose(candle->close);

	qreal decVol = 0, incVol = 0;

	QPen decPen(m_priceSeries->decreasingColor());
	QPen incPen(m_priceSeries->increasingColor());

	if (candle->close > candle->open) {
		c->setPen(incPen);
		incVol = candle->volume;
	} else {
		c->setPen(decPen);
		decVol = candle->volume;
	}

	auto volumeSets = m_volumeSeries->barSets();
	auto *incSet = volumeSets.at(0);
	auto *decSet = volumeSets.at(1);
	int lastIdx = incSet->count() - 1;
	incSet->replace(lastIdx, incVol);
	decSet->replace(lastIdx, decVol);
}

void FinChart::plotAreaChanged(const QRectF &plotArea)
{
	adjustPriceTicks(plotArea);
}

QPair<qreal, qreal> FinChart::visibleTimeRange()
{
	QRectF r = plotArea();
	auto tl = mapToValue(r.topLeft(), m_priceSeries);
	auto br = mapToValue(r.bottomRight(), m_priceSeries);
	return { tl.x(), br.x() };
}

void FinChart::rangeChanged(const QString &min, const QString &max)
{
	if (computeMinMax() < 0)
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


int FinChart::computeMinMax()
{
	auto priceSets = m_priceSeries->sets();
	auto volumeSets = m_volumeSeries->barSets();

	if (!priceSets.count())
		return -1;

	QPair<qreal, qreal> range = visibleTimeRange();

	int vmin = qRound(range.first);
	int vmax = qRound(range.second);

	if (vmin > priceSets.count() - 1 || vmax < 0)
		return -1;

	vmin = qMax(vmin, 0);
	vmax = qMin(vmax, priceSets.count() - 1);
	
	m_maxVisibleVolume = 0;
	m_maxVisiblePrice = 0;
	m_minVisiblePrice = 99999999;

	auto *incSet = volumeSets.at(0);
	auto *decSet = volumeSets.at(1);
	for (int i = vmin ; i <= vmax; i++) {
		auto *pSet = priceSets.at(i);
		m_maxVisiblePrice = qMax(pSet->high(), m_maxVisiblePrice);
		m_minVisiblePrice = qMin(pSet->low(), m_minVisiblePrice);
		qreal volume = qMax(decSet->at(i), incSet->at(i));
		m_maxVisibleVolume = qMax(volume, m_maxVisibleVolume);
	}

	return 0;
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

	QPair<qreal, qreal> range = visibleTimeRange();

	qreal plotSpan = range.second - range.first;

	if ((val > 1 && plotSpan < m_maxZoomSticks) ||
	    (val < 1 && plotSpan > m_minZoomSticks)) {
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
	auto sets = m_priceSeries->sets();
	if (!(event->buttons() & Qt::LeftButton) || !sets.count())
		return;

	auto dPos = event->pos() - m_lastMousePos;
	int dPosX = -dPos.x();
	
	QPair<qreal, qreal> range = visibleTimeRange();

	if (range.first > sets.count() - 1 && dPosX > 0) {
		dPosX = 0;
	}

	if (range.second < 0 && dPosX < 0) {
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
	applyStyle(m_preMarketShade, &preShadeStyle);
	applyStyle(m_postMarketShade, &postShadeStyle);
}

qreal FinChart::axesFontSize()
{
	return m_priceAxis->labelsFont().pixelSize();
}

void FinChart::setAxesFontSize(qreal size) {
	auto font = m_priceAxis->labelsFont();
	font.setPixelSize(size);
	m_priceAxis->setLabelsFont(font);
	m_timeAxis->setLabelsFont(font);
}

QValueAxis* FinChart::priceAxis() { return m_priceAxis; }

QValueAxis* FinChart::volumeAxis() { return m_volumeAxis; }

FinTimeAxis* FinChart::timeAxis() { return m_timeAxis; }

QCandlestickSeries* FinChart::priceSeries() { return m_priceSeries; }

QStackedBarSeries* FinChart::volumeSeries() { return m_volumeSeries; }

QDateTime FinChart::startTime() { return m_startTime; }

qint64 FinChart::unitSecSpan() { return m_unitSecSpan; }



FinChartView::FinChartView(QWidget *parent)
	: QChartView(parent)
{
	setChart(new FinChart);
	setMouseTracking(true);

	m_priceLine = new CrossHair(m_chart, Qt::Horizontal);
	m_priceLine->setDraggable(false);
	applyStyle(m_priceLine, &priceLineStyle);
	m_priceLine->setVisible(false);

	m_crossHairTime = new CrossHair(m_chart, Qt::Vertical);
	m_crossHairTime->setDraggable(false);
	m_crossHairTime->setFollowRange(false);
	applyStyle(m_crossHairTime, &crossHairStyle);

	m_crossHairPrice = new CrossHair(m_chart, Qt::Horizontal);
	m_crossHairPrice->setDraggable(false);
	m_crossHairPrice->setFollowRange(false);
	applyStyle(m_crossHairPrice, &crossHairStyle);

	m_stopLoss = new CrossHair(m_chart, Qt::Horizontal);
	m_stopLoss->setDraggable(true);
	m_stopLoss->setFollowRange(true);
	m_stopLoss->setText("stop loss");
	m_stopLoss->setSibling(m_crossHairPrice);
	applyStyle(m_stopLoss, &stopLossStyle);
	m_stopLoss->setVisible(false);

	m_takeProfit = new CrossHair(m_chart, Qt::Horizontal);
	m_takeProfit->setDraggable(true);
	m_takeProfit->setFollowRange(true);
	m_takeProfit->setText("take profit");
	m_takeProfit->setSibling(m_crossHairPrice);
	applyStyle(m_takeProfit, &takeProfitStyle);
	m_takeProfit->setVisible(false);
}

CrossHair* FinChartView::stopLoss() { return m_stopLoss; }
CrossHair* FinChartView::takeProfit() { return m_takeProfit; }

void FinChartView::updatePriceLine()
{
	auto last = m_chart->priceSeries()->sets().last();
	bool isUp = last->close() >= last->open();
	m_priceLine->setIsUp(isUp);
	m_priceLine->setValue(QPointF(1, last->close()));
}

void FinChartView::setCandles(const QList<Candle *> &candles, int interval)
{
	m_chart->timeAxis()->setCandleInterval(interval);
	m_chart->setCandles(candles);
	updatePriceLine();
	m_priceLine->setVisible(true);
	m_stopLoss->setVisible(true);
	m_takeProfit->setVisible(true);

	m_latestCandle = *candles.last();
}

void FinChartView::setLatestCandle(Candle candle)
{
	if (m_latestCandle.time == candle.time)
		m_chart->editCandle(&candle);
	else
		m_chart->appendCandle(&candle);

	m_latestCandle = candle;
	updatePriceLine();
}

FinChart* FinChartView::chart() { return m_chart; }

void FinChartView::setChart(FinChart *chart)
{
	m_chart = chart;
	QChartView::setChart(chart);
}

void FinChartView::mouseMoveEvent(QMouseEvent *event)
{
	m_chart->mouseMoveEvent(event);

	m_crossHairTime->setPos(event->pos());
	m_crossHairPrice->setPos(event->pos());

	QChartView::mouseMoveEvent(event);
}

void FinChartView::mousePressEvent(QMouseEvent *event)
{
	m_chart->mousePressEvent(event);
	QChartView::mousePressEvent(event);
}

void FinChartView::wheelEvent(QWheelEvent *event)
{
	m_chart->wheelEvent(event);
	QChartView::wheelEvent(event);
}
