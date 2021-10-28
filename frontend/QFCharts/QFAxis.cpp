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


#include "QFAxis.h"

#include "QFCandle.h"
#include "QFChart.h"
#include "QFSeries.h"

#include <QtGui/QPainter>


static const QList<int> k_niceMinuteIntervals = { 60, 30, 15, 10, 5 };
static const QList<int> k_niceDecimalIntervals = { 1, 2, 5, 10 };


QFAbstractAxis::QFAbstractAxis(QQuickItem *parent)
	: QQuickPaintedItem(parent),
	m_parentAxis(nullptr),
	m_orientation(Qt::Horizontal),
	m_prevRangeLength(0),
	m_min(0),
	m_max(1),
	m_gridLineWidth(1),
	m_tickSize(3),
	m_gridLineColor(Qt::darkGray),
	m_color(Qt::black),
	m_labelsColor(Qt::black),
	m_labelsVisible(true),
	m_labelsSpace(52)
{
	setAntialiasing(true);
}

void QFValueAxis::paint(QPainter* painter)
{
	QFAbstractAxis::paint(painter);
	if (!labelsVisible() || labelsSpace() < 1)
		return;
	auto metrics = QFontMetrics(m_labelsFont);
	qreal textHeight = metrics.capHeight() * 1.5;
	qreal textWidth = metrics.maxWidth() * 5;
	qreal textHalfHeight = textHeight / 2;

	painter->setFont(m_labelsFont);
	painter->setPen(m_labelsColor);

	for (int i = 0; i < m_labels.size(); i++) {
		qreal tick = m_ticks.at(i);
		if (tick < min() || tick > max())
			continue;
		const QString& label = m_labels.at(i);
		qreal tickPos = mapValue(tick);
		auto rect = QRectF(width() - labelsSpace() + m_tickSize + 2,
			tickPos - textHalfHeight, textWidth, textHeight);
		painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, label);
	}
}

void QFTimeAxis::paint(QPainter* painter)
{
	QFAbstractAxis::paint(painter);
	if (!labelsVisible() || labelsSpace() < 1)
		return;
	painter->setPen(m_labelsColor);
	QFont boldFont = m_labelsFont;
	boldFont.setBold(true);
	painter->setFont(m_labelsFont);
	auto metrics = QFontMetrics(m_labelsFont);
	qreal textHeight = metrics.capHeight() * 1.5;
	qreal textWidth = metrics.maxWidth() * 5;
	qreal textHalfWidth = textWidth / 2;
	for (int i = 0; i < m_labels.size(); i++) {
		qreal tick = m_ticks.at(i);
		if (tick < min() || tick > max())
			continue;
		const QString& label = m_labels.at(i);
		qreal tickPos = mapValue(tick);
		auto rect = QRectF(tickPos - textHalfWidth,
			height() - labelsSpace() + m_tickSize, textWidth, textHeight);
		if (m_milestoneTicks.contains(tick))
			painter->setFont(boldFont);
		painter->drawText(rect, Qt::AlignCenter, label);
		if (painter->font() == boldFont)
			painter->setFont(m_labelsFont);
	}
}

void QFAbstractAxis::paint(QPainter* painter)
{
	qreal labelsSpace = labelsVisible() ? this->labelsSpace() : 0;

	painter->setRenderHints(painterHints);
	painter->setPen(Qt::NoPen);

	QRectF borderLine;
	QRectF axisLine;
	if (m_orientation == Qt::Horizontal) {
		borderLine.setWidth(width());
		borderLine.setHeight(m_gridLineWidth);
		axisLine.setY(height() - labelsSpace - m_gridLineWidth);
		axisLine.setWidth(width());
		axisLine.setHeight(m_gridLineWidth);
	} else {
		borderLine.setWidth(m_gridLineWidth);
		borderLine.setHeight(height());
		axisLine.setX(width() - labelsSpace - m_gridLineWidth);
		axisLine.setWidth(m_gridLineWidth);
		axisLine.setHeight(height());
	}

	painter->setBrush(m_color);
	painter->drawRect(borderLine);
	painter->drawRect(axisLine);

	if (m_ticks.empty())
		return;

	qreal halfGridLineWidth = m_gridLineWidth / 2;
	qreal gridLineWidth2 = m_gridLineWidth * 2;
	QList<QRectF> gridLineRects;
	QList<QRectF> tickRects;
	foreach (qreal tick, m_ticks) {
		if (tick < min() || tick > max())
			continue;
		qreal tickPos = mapValue(tick);
		QRectF gridLine;
		QRectF tickLine;
		if (m_orientation == Qt::Horizontal) {
			gridLine.setX(tickPos);
			gridLine.setY(m_gridLineWidth);
			gridLine.setWidth(m_gridLineWidth);
			gridLine.setHeight(height() - labelsSpace - gridLineWidth2);
			if (labelsSpace > 1) {
				tickLine.setX(gridLine.x());
				tickLine.setY(gridLine.height() + gridLineWidth2);
				tickLine.setWidth(gridLine.width());
				tickLine.setHeight(m_tickSize);
			}
		} else {
			gridLine.setY(height() - tickPos - halfGridLineWidth);
			gridLine.setHeight(m_gridLineWidth);
			gridLine.setWidth(width() - labelsSpace);
			if (labelsSpace > 1) {
				tickLine.setY(gridLine.y());
				tickLine.setX(gridLine.width());
				tickLine.setHeight(gridLine.height());
				tickLine.setWidth(m_tickSize);
			}
		}
		gridLineRects.push_back(gridLine);
		tickRects.push_back(tickLine);
	}

	if (labelsSpace > 1)
		painter->drawRects(tickRects);
	painter->setBrush(m_gridLineColor);
	painter->drawRects(gridLineRects);
}


void QFAbstractAxis::componentComplete()
{
	if (m_parentAxis)
		connectParentAxis();
	QQuickItem::componentComplete();
}


void QFAbstractAxis::zoom(const QRectF& rectangle)
{
	qreal dx = mapRanges(rectangle.x(), 0, 1, x(), x() + width());
	qreal dy = mapRanges(rectangle.y(), 0, 1, y(), y() + height());
	qreal sx = rectangle.width() / width();
	qreal sy = rectangle.height() / height();
	m_zoom.translate(dx, dy);
	m_zoom.scale(sx, sy);
	zoomToRange();
	setRange(m_min, m_max);
}

void QFAbstractAxis::zoomToRange()
{
	auto range = m_zoom.mapRect(QRectF(0, 0, 1, 1));
	if (m_orientation == Qt::Horizontal) {
		m_min = range.left();
		m_max = range.right();
	} else {
		m_min = range.top();
		m_max = range.bottom();
	}
}

void QFAbstractAxis::rangeToZoom()
{
	auto newZoom = QTransform();
	qreal d = m_min;
	qreal s = m_max - m_min;
	if (m_orientation == Qt::Horizontal) {
		newZoom.translate(d, m_zoom.m32());
		newZoom.scale(s, m_zoom.m22());
	} else {
		newZoom.translate(m_zoom.m31(), d);
		newZoom.scale(m_zoom.m11(), s);
	}
	m_zoom = newZoom;
}


void QFAbstractAxis::connectChart(QFChart *chart)
{
	if (!chart)
		return;
	// one axis can belong to several series, prevent redundant updates
	if (!m_plotAreaChangedConnection) {
		m_plotAreaChangedConnection = connect(chart, &QFChart::plotAreaChanged,
			this, &QFAbstractAxis::onPlotAreaChanged);
	}
	if (!m_chartZoomChangedConnection) {
		m_chartZoomChangedConnection = connect(chart, &QFChart::zoomChanged,
			this, &QFAbstractAxis::zoom);
	}
}

void QFAbstractAxis::disconnectChart(QFChart *chart)
{
	if (!chart)
		return;
	if (m_plotAreaChangedConnection) {
		disconnect(chart, &QFChart::plotAreaChanged,
			this, &QFAbstractAxis::onPlotAreaChanged);
		m_plotAreaChangedConnection = QMetaObject::Connection();
	}
	if (m_chartZoomChangedConnection) {
		disconnect(chart, &QFChart::zoomChanged,
			this, &QFAbstractAxis::zoom);
		m_chartZoomChangedConnection = QMetaObject::Connection();
	}
}

void QFAbstractAxis::onPlotAreaChanged(const QRectF& plotArea)
{
	qreal labelsSpace = labelsVisible() ? this->labelsSpace() : 0;
	if (m_orientation == Qt::Horizontal) {
		setWidth(plotArea.width());
		setHeight(plotArea.height() + labelsSpace);
	} else {
		setWidth(plotArea.width() + labelsSpace);
		setHeight(plotArea.height());
	}
}

void QFAbstractAxis::setRange(qreal min, qreal max)
{
	m_min = min;
	m_max = max;
	computeTicks();
	rangeToZoom();
	emit rangeChanged(m_min, m_max);
	update();
}

void QFAbstractAxis::setMin(qreal min)
{
	setRange(min, m_max);
}

void QFAbstractAxis::setMax(qreal max)
{
	setRange(m_min, max);
}

void QFAbstractAxis::connectParentAxis()
{
	if (!m_parentAxis)
		return;
	connect(m_parentAxis, &QFAbstractAxis::rangeChanged,
		this, &QFAbstractAxis::setRange);
	setRange(m_parentAxis->min(), m_parentAxis->max());

	connect(m_parentAxis, &QFAbstractAxis::ticksChanged,
		this, &QFAbstractAxis::computeTicks);
}

qreal QFAbstractAxis::mapValue(qreal v) {
	qreal r = mapRanges(v, 0, m_orientation == Qt::Horizontal ? width() : height(), m_min, m_max);
	if (m_orientation == Qt::Vertical)
		r = height() - r;
	return r;
}

qreal QFAbstractAxis::mapPosition(qreal p) {
	if (m_orientation == Qt::Vertical)
		p = height() - p;
	return mapRanges(p, m_min, m_max, 0, m_orientation == Qt::Horizontal ? width() : height());
}

qreal QFAbstractAxis::mapRanges(qreal v, qreal outMin, qreal outMax, qreal inMin, qreal inMax) {
	//return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	return (v - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

qreal QFAbstractAxis::labelsSpace() { return m_labelsSpace; }

void QFAbstractAxis::setLabelsSpace(qreal space) {
	m_labelsSpace = space;
	emit labelsSpaceChanged(space);
}

bool QFAbstractAxis::labelsVisible() { return m_labelsVisible; }

void QFAbstractAxis::setLabelsVisible(bool visible) {
	m_labelsVisible = visible;
	emit labelsVisibleChanged(visible);
}


qreal QFAbstractAxis::gridLineWidth() { return m_gridLineWidth; }

Qt::Orientation QFAbstractAxis::orientation() { return m_orientation; }

qreal QFAbstractAxis::min() { return m_min; }

qreal QFAbstractAxis::max() { return m_max; }

const QList<qreal>& QFAbstractAxis::ticks() const { return m_ticks; }

const QList<QString>& QFAbstractAxis::labels() const { return m_labels; }

void QFAbstractAxis::computeTicks() {}

QFTimeAxis::QFTimeAxis(QQuickItem *parent)
	:QFAbstractAxis(parent),
	m_chart(nullptr),
	m_prevCandleCount(0)
{
	m_minimumTickInterval = 70;
	setLabelsSpace(18);
}


void QFTimeAxis::setChart(QFChart* chart) {
	m_chart = chart;
	connect(m_chart, &QFChart::candlesChanged,
		this, &QFAbstractAxis::computeTicks);
}

const QList<qreal>& QFTimeAxis::milestoneTicks() const { return m_milestoneTicks; }

void QFTimeAxis::computeTicks() {
	if (m_parentAxis) {
		m_ticks = m_parentAxis->ticks();
		m_labels = m_parentAxis->labels();
		auto timeAxis = qobject_cast<QFTimeAxis *>(m_parentAxis);
		if (timeAxis)
			m_milestoneTicks = timeAxis->milestoneTicks();
		return;
	}
	qreal curRangeLength = qRound((max() - min()) * 100) / 100;
	int curCandleCount = m_chart ? m_chart->candles().size() : 0;

	bool lenChanged = curRangeLength != m_prevRangeLength;
	bool countChanged = curCandleCount != m_prevCandleCount;

	m_prevRangeLength = curRangeLength;
	m_prevCandleCount = curCandleCount;

	if (!lenChanged && !countChanged)
		return;

	if (!m_chart || m_chart->candles().size() < 2
		|| !m_chart->candles().first().canConvert<QFCandle>())
		return;

	int selectedInterval = m_chart->selectedInterval();
	qreal maxTickCount = floor(width() / m_minimumTickInterval);
	int lastIdx = m_chart->candles().size() - 1;
	int niceTickInterval;
	int niceAnchorIdx = -1;
	
	if (selectedInterval < 60) {
		qreal tickInterval = (max() - min()) / maxTickCount * selectedInterval;

		int hourlyInterval = tickInterval > 60 ?
			ceil(tickInterval / 60) * 60 : 9999999;

		QList<int> niceIntervals = QList<int>(k_niceMinuteIntervals);
		niceIntervals.push_front(hourlyInterval);
		niceIntervals.push_back(-9999999);

		int niceIdx = -1;
		for (int i = 0; i < niceIntervals.size(); i++) {
			if (tickInterval > niceIntervals.at(i)) {
				niceIdx = i;
				break;
			}
		}
		if (niceIdx <= 0)
			return;
		niceTickInterval = niceIntervals.at(niceIdx - 1);
		for (int i = lastIdx; i >= 0; i--) {
			auto c = m_chart->candles().at(i).value<QFCandle>();
			auto dt = c.time;
			int minutes = dt.time().minute();
			if (minutes % niceTickInterval == 0) {
				niceAnchorIdx = i;
				break;
			}
		}
		niceTickInterval = qRound((qreal)niceTickInterval / selectedInterval);
	} else {
		niceTickInterval = qMax((int)qRound((max() - min()) / maxTickCount), 1);
		for (int i = lastIdx; i >= 0; i--) {
			if (i % niceTickInterval == 0) {
				niceAnchorIdx = i;
				break;
			}
		}
	}

	if (niceAnchorIdx < 0 || niceTickInterval == 0)
		return;

	QList<qreal> ticks;
	QList<qreal> milestoneTicks;

	ticks.push_back(lastIdx);
	int startIdx = lastIdx - niceTickInterval - 1;
	for (int i = startIdx; i >= 0; i--) {
		auto c = m_chart->candles().at(i).value<QFCandle>();
		auto dt = c.time;
		int minute = dt.time().minute();
		int hour = dt.time().hour();
		int day = dt.date().day() - 1;
		int month = dt.date().month() - 1;
		if ((selectedInterval < 1440 && !(minute + hour))
		    || (selectedInterval < 99999 && !day) || !month) {
			if (!ticks.empty()
			    && ticks.back() - i < niceTickInterval)
			    ticks.pop_back();
			ticks.push_back(i);
			milestoneTicks.push_back(i);
			i -= niceTickInterval - 1;
		} else if ((niceAnchorIdx - i) % niceTickInterval == 0) {
			ticks.push_back(i);
		}
	}

	std::reverse(ticks.begin(), ticks.end());
	m_ticks = ticks;
	m_milestoneTicks = milestoneTicks;
	computeLabels();
	emit ticksChanged();
}


void QFTimeAxis::computeLabels()
{
	m_labels.clear();
	if (m_ticks.empty())
		return;

	int selectedInterval = m_chart->selectedInterval();
	auto format = QString("hh:mm");
	if (selectedInterval > 10080)
		format = "MMM";
	else if (selectedInterval >= 1440)
		format = "dd";

	foreach (qreal tick, m_ticks) {
		int idx = (int)tick;
		auto dt = m_chart->candles().at(idx).value<QFCandle>().time;
		QString label;
		if (m_milestoneTicks.contains(tick)) {
			int hour = dt.time().hour();
			int day = dt.date().day() - 1;
			int month = dt.date().month() - 1;
			auto milestoneFormat = QString("dd");
			if (month == 0)
				milestoneFormat = "yyyy";
			else if (day == 0)
				milestoneFormat = "MMM";
			else if (hour == 0)
				milestoneFormat = "dd";
			label = dt.toString(milestoneFormat);
		} else {
			label = dt.toString(format);
		}
		m_labels.push_back(label);
	}
}

QFValueAxis::QFValueAxis(QQuickItem *parent)
	: QFAbstractAxis(parent),
	m_paddingPlotFraction(0.07),
	m_labelFormat("%.2f")
{
	m_orientation = Qt::Vertical;
	m_minimumTickInterval = 40;
}

void QFValueAxis::niceZoom(qreal minValue, qreal maxValue)
{
	qreal valueRange = maxValue - minValue;
	if (valueRange <= 0)
		return;

	qreal tickPadding = valueRange * m_paddingPlotFraction;

	qreal plotPadding = height() * m_paddingPlotFraction;
	qreal effectiveHeight = height() - plotPadding * 2;

	qreal maxTickCount = floor(effectiveHeight / m_minimumTickInterval);
	qreal minValueInterval = valueRange / maxTickCount;
	qreal decimalMultiplier = pow(10, floor(log10(minValueInterval)));
	qreal normalizedInterval = minValueInterval / decimalMultiplier;

	qreal niceNormalizedInterval = 10;
	foreach (int nd, k_niceDecimalIntervals) {
		if (nd >= normalizedInterval) {
			niceNormalizedInterval = (qreal)nd;
			break;
		}
	}

	qreal tickInterval = niceNormalizedInterval * decimalMultiplier;
	qreal newMin = floor(minValue / tickInterval) * tickInterval;
	qreal newMax = ceil(maxValue / tickInterval) * tickInterval;
	m_ticks.clear();
	for (qreal tick = newMin; tick <= newMax; tick += tickInterval) {
		m_ticks.push_back(tick);
	}
	setRange(newMin - tickPadding, newMax + tickPadding);
	computeLabels();
}


	void QFValueAxis::computeLabels()
{
	m_labels.clear();
	if (m_ticks.empty())
		return;
	foreach (qreal tick, m_ticks)
		m_labels.push_back(QString::asprintf(m_labelFormat.toLatin1().data(), tick));
}
