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


#include "shade.h"
#include "finchartview.h"
#include "fintimeaxis.h"

#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>


Shade::Shade(FinChart *chart):
	QObject(),
	QGraphicsRectItem(chart),
	m_chart(chart)
{
	updateColor();

	connect(m_chart, &QChart::plotAreaChanged, this, &Shade::rangeChanged);
	auto *axisPrice = m_chart->priceAxis();
	connect(axisPrice, &QValueAxis::rangeChanged, this, &Shade::rangeChanged);
	auto *axisTime = m_chart->timeAxis();
	connect(axisTime, &FinTimeAxis::layoutChanged, this, &Shade::rangeChanged);

	auto *priceSeries = m_chart->priceSeries();
	connect(priceSeries, &QCandlestickSeries::candlestickSetsAdded, this, &Shade::priceSeriesAdded);
}

void Shade::rangeChanged()
{
	updateRect();
}

void Shade::priceSeriesAdded()
{
	updateTimeRangeF();
}

qreal Shade::mapX(qreal x)
{
	return m_chart->mapToPosition(QPointF(x, 0),
		(QAbstractSeries *)m_chart->priceSeries()).x();
}

qreal Shade::mapY(qreal y)
{
	return m_chart->mapToPosition(QPointF(0, y),
		(QAbstractSeries *)m_chart->priceSeries()).y();
}

void Shade::updateRect()
{
	if ((m_orientation == Qt::Horizontal
	     && qIsNaN(m_priceRange.first) && qIsNaN(m_priceRange.second))
	    || (m_orientation == Qt::Vertical
	     && qIsNaN(m_timeRangeF.first) && qIsNaN(m_timeRangeF.second)))
		return;

	qreal x, y, w, h;

	QRectF area = m_chart->plotArea();

	if (m_orientation == Qt::Horizontal) {
		x = area.x();
		w = area.width();
		y = qMax(mapY(m_priceRange.second), area.y());
		h = qMin(mapY(m_priceRange.first) - y, area.bottom() - y);
	} else {
		y = area.y();
		h = area.height();
		x = qMax(mapX(m_timeRangeF.first), area.x());
		w = qMin(mapX(m_timeRangeF.second) - x, area.right() - x);
	}

	auto r = QRectF(x, y, w, h);

	if (!area.intersects(r)) {
		setVisible(false);
		return;
	}

	setVisible(true);
	QGraphicsRectItem::setRect(r);
	prepareGeometryChange();
}

void Shade::setTimeRange(QDateTime min, QDateTime max)
{
	m_orientation = Qt::Vertical;
	m_timeRange = { min, max };
	updateTimeRangeF();
	updateRect();
}

QPair<QDateTime, QDateTime> Shade::timeRange() { return m_timeRange; }

void Shade::updateTimeRangeF()
{
	if (m_timeRange.first.isNull() || m_timeRange.first.isNull() ||
	    !m_chart->priceSeries()->count())
		return;

	auto sets = m_chart->priceSeries()->sets();

	auto lb = [] (QCandlestickSet *set, QDateTime value) {
		return QDateTime::fromMSecsSinceEpoch(set->timestamp()) < value;
	};

	QList<QCandlestickSet *>::iterator l = std::lower_bound(
		sets.begin(), sets.end(), m_timeRange.first, lb
	);

	m_timeRangeF.first = l == sets.end() ? qQNaN() : l - sets.begin();

	auto ub = [] (QCandlestickSet *set, QDateTime value) {
		return QDateTime::fromMSecsSinceEpoch(set->timestamp()) > value;
	};

	QList<QCandlestickSet *>::reverse_iterator u = std::lower_bound(
		 sets.rbegin(), sets.rend(), m_timeRange.second, ub
	);

	m_timeRangeF.second = u == sets.rend() ? qQNaN() : sets.rend() - u - 1;
}

void Shade::setPriceRange(qreal min, qreal max)
{
	m_orientation = Qt::Horizontal;
	m_priceRange = { min, max };
	updateRect();
}

void Shade::updateColor()
{
	QGraphicsRectItem::setPen(Qt::NoPen);
	QGraphicsRectItem::setBrush(m_color);
}

QColor Shade::color() { return m_color; }
void Shade::setColor(QColor color) { m_color = color; updateColor(); }
