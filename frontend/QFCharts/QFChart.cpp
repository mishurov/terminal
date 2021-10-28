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


#include "QFChart.h"

#include "QFSeries.h"
#include "QFAxis.h"

#include <QtQuick/QSGSimpleRectNode>


QFChart::QFChart(QQuickItem *parent)
	:QQuickItem(parent),
	m_axisX(nullptr),
	m_axisY(nullptr),
	m_selectedInterval(1)
{
	setFlag(QQuickItem::ItemHasContents, true);
}

void QFChart::componentComplete() {
	foreach (QObject *child, children()) {
		if (auto series = qobject_cast<QFAbstractSeries *>(child))
			addSeries(series);
		if (auto axis = qobject_cast<QFAbstractAxis *>(child)) {
			bool axisAdded = false;
			if (!m_axisX && axis->orientation() == Qt::Horizontal) {
				m_axisX = axis;
				axisAdded = true;
			}
			if (!m_axisY && axis->orientation() == Qt::Vertical) {
				m_axisY = axis;
				axisAdded = true;
			}
			if (axisAdded) {
				connect(axis, &QFAbstractAxis::labelsSpaceChanged,
					this, &QFChart::onLabelsStateChanged);
				connect(axis, &QFAbstractAxis::labelsVisibleChanged,
					this, &QFChart::onLabelsStateChanged);
			}
			auto timeAxis = qobject_cast<QFTimeAxis *>(axis);
			if (timeAxis)
				timeAxis->setChart(this);
		}
	}

	QQuickItem::componentComplete();
}

// TODO background color
/*
QSGNode* QFChart::updatePaintNode(QSGNode *node, UpdatePaintNodeData *data)
{
	QSGSimpleRectNode *n = static_cast<QSGSimpleRectNode *>(node);
	if (!n) {
		n = new QSGSimpleRectNode();
		n->setColor(Qt::gray);
	}
	n->setRect(boundingRect());
	return n;
}
*/

void QFChart::scrollLeft(qreal pixels)
{
	auto rectangle = QRectF(-pixels, 0, m_plotArea.width(), m_plotArea.height());
	emit zoomChanged(rectangle);
}

void QFChart::zoomIn(QRectF &rectangle)
{
	rectangle.moveLeft(rectangle.x() - x());
	rectangle.moveTop(rectangle.y() - y());
	emit zoomChanged(rectangle);
}

void QFChart::geometryChange(const QRectF &newGeometry,
	const QRectF &oldGeometry)
{
	if (newGeometry.isValid() && newGeometry.width() > 0 && newGeometry.height() > 0) {
		setPlotAreaSize(newGeometry.width(), newGeometry.height());
	}
	QQuickItem::geometryChange(newGeometry, oldGeometry);
}

void QFChart::onLabelsStateChanged()
{
	setPlotAreaSize(width(), height());
}

void QFChart::setPlotAreaSize(qreal chartWidth, qreal chartHeight)
{
	qreal plotWidth = 0;
	qreal plotHeight = 0;
	if (m_axisY && m_axisY->labelsVisible())
		plotWidth -= m_axisY->labelsSpace();
	if (m_axisX && m_axisX->labelsVisible())
		plotHeight -= m_axisX->labelsSpace();
	plotWidth += chartWidth;
	plotHeight += chartHeight;
	if (plotWidth < 0 || plotHeight < 0)
		return;
	m_plotArea.setWidth(plotWidth);
	m_plotArea.setHeight(plotHeight);
	emit plotAreaChanged(m_plotArea);
}

const QList<QVariant>& QFChart::candles() const { return m_candles; }

void QFChart::setCandles(const QList<QVariant>& candles) {
	m_candles = candles;
	emit candlesChanged();
}

void QFChart::addSeries(QFAbstractSeries *series)
{
	if (series) {
		m_series.append(series);
		series->setChart(this);
		if (!series->axisX() && m_axisX)
			series->setAxisX(m_axisX);
		if (!series->axisY() && m_axisY)
			series->setAxisY(m_axisY);
		// emit for the series added later
		setPlotAreaSize(width(), height());
	} else {
		qWarning("addSeries: can not add nullptr");
	}
}

void QFChart::removeSeries(QFAbstractSeries *series)
{
	if (m_series.removeOne(series)) {
		series->removeChart();
	} else {
		qWarning("removeSeries: unable to remove series");
	}
}

QPointF QFChart::mapToPosition(QPointF v)
{
	QPointF ret;
	if (!m_axisX || !m_axisY)
		return ret;
	ret.setX(m_axisX->mapValue(v.x()));
	ret.setY(m_axisY->mapValue(v.y()));
	return ret;
}

QPointF QFChart::mapToValue(QPointF p)
{
	QPointF ret;
	if (!m_axisX || !m_axisY)
		return ret;
	ret.setX(m_axisX->mapPosition(p.x()));
	ret.setY(m_axisY->mapPosition(p.y()));
	return ret;
}

int QFChart::selectedInterval() { return m_selectedInterval; }

void QFChart::setSelectedInterval(int interval)
{
	m_selectedInterval = interval;
}

QFAbstractAxis* QFChart::axisX() { return m_axisX; }

QFAbstractAxis* QFChart::axisY() { return m_axisY; }

const QRectF& QFChart::plotArea() const { return m_plotArea; }
