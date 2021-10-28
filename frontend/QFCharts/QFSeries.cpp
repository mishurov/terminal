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


#include "QFSeries.h"

#include "QFCandle.h"
#include "QFChart.h"
#include "QFAxis.h"
#include "QFGeoNodes.h"

#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QQuickWindow>


QFLines::QFLines(QQuickItem *parent)
	: QFAbstractSeries(parent),
	m_lineColor(Qt::blue),
	m_lineWidth(1)
{
}

QSGNode* QFLines::updatePaintNode(QSGNode *node, UpdatePaintNodeData *data)
{
	if (dataset().empty())
		return node;

	// TODO refactor, too many repetitions
	QSGNode *n = node;
	QSGClipNode *cn = nullptr;
	QFPolyLineNode *polyLine = nullptr;
	QSGSimpleTextureNode *tn = nullptr;
	if (!n) {
		if (m_isRhi) {
			cn = new QSGClipNode();
			cn->setIsRectangular(true);
			polyLine = new QFPolyLineNode();
			polyLine->setColor(m_lineColor);
			polyLine->setLineWidth(m_lineWidth);
			cn->appendChildNode(polyLine);
		} else {
			n = new QSGNode();
			tn = new QSGSimpleTextureNode();
			n->appendChildNode(tn);
		}
	} else {
		if (m_isRhi) {
			cn = static_cast<QSGClipNode *>(node);
			polyLine = static_cast<QFPolyLineNode *>(cn->childAtIndex(0));
		} else {
			tn = static_cast<QSGSimpleTextureNode *>(n->childAtIndex(0));
		}

	}


	QList<QPointF> points;
	for (int i = minVisibleIdx(); i <= maxVisibleIdx(); i++) {
		auto pt = dataset().at(i).value<QPointF>();
		qreal ptX = m_axisX->mapValue(pt.x());
		qreal ptY = m_axisY->mapValue(pt.y());
		points.append(QPointF(ptX, ptY));
	}


	if (m_isRhi) {
		cn->setClipRect(getClipRect());
		polyLine->setPoints(points);
		return cn;
	}

	m_plotImage->fill(Qt::transparent);
	QPainter painter(m_plotImage);
	painter.setRenderHints(painterHints);
	painter.setClipRect(getClipRect());
	QPen pen;
	pen.setColor(m_lineColor);
	pen.setWidth(m_lineWidth);
	painter.setPen(pen);
	painter.drawPolyline(points.data(), points.size());

	QSGTexture *texture = window()->createTextureFromImage(*m_plotImage);
	tn->setTexture(texture);
	tn->setRect(0, 0, width(), height());

	return n;
}

qreal QFLines::startOffset() {
	return !dataset().empty() ? dataset().first().value<QPointF>().x() : 0;
}

int QFLines::minVisibleIdx()
{
	return m_axisX ? qMax((int)round(m_axisX->min() - 1 - startOffset()), 0): 0;
}

int QFLines::maxVisibleIdx()
{
	if (!m_axisX || dataset().empty())
		return 0;
	return qMin((int)round(m_axisX->max() + 1 - startOffset()), dataset().size() - 1);
}

QFVolumes::QFVolumes(QQuickItem *parent)
	: QFCandlesSeriesMixin(parent),
	m_barWidth(0.9)
{}

QSGNode* QFVolumes::updatePaintNode(QSGNode *node, UpdatePaintNodeData *data)
{
	if (dataset().empty())
		return node;

	QSGNode *n = node;
	QSGClipNode *cn = nullptr;
	QFRectsNode *barsUp = nullptr;
	QFRectsNode *barsDown = nullptr;
	QSGSimpleTextureNode *tn = nullptr;

	if (!n) {
		if (m_isRhi) {
			cn = new QSGClipNode();
			cn->setIsRectangular(true);
			barsUp = new QFRectsNode();
			barsDown = new QFRectsNode();
			barsUp->setColor(m_increasingColor);
			barsDown->setColor(m_decreasingColor);
			cn->appendChildNode(barsUp);
			cn->appendChildNode(barsDown);
		} else {
			n = new QSGNode();
			tn = new QSGSimpleTextureNode();
			n->appendChildNode(tn);
		}
	} else {
		if (m_isRhi) {
			cn = static_cast<QSGClipNode *>(node);
			barsUp = static_cast<QFRectsNode *>(cn->childAtIndex(0));
			barsDown = static_cast<QFRectsNode *>(cn->childAtIndex(1));
		} else {
			tn = static_cast<QSGSimpleTextureNode *>(n->childAtIndex(0));
		}
	}

	qreal barWidth = m_itemHSpacing * m_barWidth;
	qreal barHalfWidth = barWidth / 2;
	QList<QRectF> barsUpRects;
	QList<QRectF> barsDownRects;

	for (int i = minVisibleIdx(); i <= maxVisibleIdx(); i++) {
		auto candle = dataset().at(i).value<QFCandle>();
		qreal barX = m_axisX->mapValue((qreal)i) - barHalfWidth;
		qreal volume = m_axisY->mapValue(candle.volume);
		auto bar = QRectF(barX, volume, barWidth, volume);
		bool isUp = candle.open < candle.close;
		QList<QRectF>& container = isUp ? barsUpRects : barsDownRects;
		container.append(bar);
	}

	if (m_isRhi) {
		cn->setClipRect(getClipRect());
		barsUp->setRects(barsUpRects);
		barsDown->setRects(barsDownRects);
		return cn;
	}

	m_plotImage->fill(Qt::transparent);
	QPainter painter(m_plotImage);
	painter.setRenderHints(painterHints);
	painter.setClipRect(getClipRect());
	painter.setPen(Qt::NoPen);
	painter.setBrush(m_increasingColor);
	painter.drawRects(barsUpRects);
	painter.setBrush(m_decreasingColor);
	painter.drawRects(barsDownRects);

	QSGTexture *texture = window()->createTextureFromImage(*m_plotImage);
	tn->setTexture(texture);
	tn->setRect(0, 0, width(), height());

	return n;
}

QFCandles::QFCandles(QQuickItem *parent)
	: QFCandlesSeriesMixin(parent),
	m_bodyWidth(0.8),
	m_shadowWidth(0.1)
{}

QSGNode* QFCandles::updatePaintNode(QSGNode *node, UpdatePaintNodeData *data)
{
	if (dataset().empty())
		return node;

	QSGNode *n = node;
	QSGClipNode *cn = nullptr;
	QFRectsNode *candlesUp = nullptr;
	QFRectsNode *candlesDown = nullptr;
	QSGSimpleTextureNode *tn = nullptr;
	// clip node doesn't resize with software backend
	if (!n) {
		if (m_isRhi) {
			cn = new QSGClipNode();
			cn->setIsRectangular(true);
			candlesUp = new QFRectsNode();
			candlesDown = new QFRectsNode();
			candlesUp->setColor(m_increasingColor);
			candlesDown->setColor(m_decreasingColor);
			cn->appendChildNode(candlesUp);
			cn->appendChildNode(candlesDown);
		} else {
			n = new QSGNode();
			tn = new QSGSimpleTextureNode();
			n->appendChildNode(tn);
		}
	} else {
		if (m_isRhi) {
			cn = static_cast<QSGClipNode *>(n);
			candlesUp = static_cast<QFRectsNode *>(n->childAtIndex(0));
			candlesDown = static_cast<QFRectsNode *>(n->childAtIndex(1));
		} else {
			tn = static_cast<QSGSimpleTextureNode *>(n->childAtIndex(0));
		}
	}

	qreal bodyWidth = m_itemHSpacing * m_bodyWidth;
	qreal shadowWidth = qMin(m_itemHSpacing * m_shadowWidth, 1.0);
	qreal bodyHalfWidth = bodyWidth / 2;
	qreal shadowHalfWidth = shadowWidth / 2;
	QList<QRectF> candlesUpRects;
	QList<QRectF> candlesDownRects;
	QRectF body;
	QRectF shadow;

	for (int i = minVisibleIdx(); i <= maxVisibleIdx(); i++) {
		auto candle = dataset().at(i).value<QFCandle>();
		qreal candleX = m_axisX->mapValue((qreal)i);
		qreal bodyX = candleX - bodyHalfWidth;
		qreal shadowX = candleX - shadowHalfWidth;
		qreal open = m_axisY->mapValue(candle.open);
		qreal close = m_axisY->mapValue(candle.close);
		qreal high = m_axisY->mapValue(candle.high);
		qreal low = m_axisY->mapValue(candle.low);
		bool isUp = candle.open < candle.close;
		qreal bodyHeight = qMax(qAbs(open - close), 1.0);
		body = QRectF(bodyX, qMin(open, close), bodyWidth, bodyHeight);
		shadow = QRectF(shadowX, high, shadowWidth, low - high);
		QList<QRectF>& candlesContainer = isUp ? candlesUpRects : candlesDownRects;
		candlesContainer.append(shadow);
		candlesContainer.append(body);
	}

	if (m_isRhi) {
		cn->setClipRect(getClipRect());
		candlesUp->setRects(candlesUpRects);
		candlesDown->setRects(candlesDownRects);
		return cn;
	}

	m_plotImage->fill(Qt::transparent);
	QPainter painter(m_plotImage);
	painter.setRenderHints(painterHints);
	painter.setClipRect(getClipRect());
	painter.setPen(Qt::NoPen);
	painter.setBrush(m_increasingColor);
	painter.drawRects(candlesUpRects);
	painter.setBrush(m_decreasingColor);
	painter.drawRects(candlesDownRects);

	QSGTexture *texture = window()->createTextureFromImage(*m_plotImage);
	tn->setRect(0, 0, width(), height());
	tn->setTexture(texture);

	return n;
}

QFCandlesSeriesMixin::QFCandlesSeriesMixin(QQuickItem *parent)
	:QFAbstractSeries(parent),
	m_increasingColor(Qt::green),
	m_decreasingColor(Qt::red)
{
}

void QFCandlesSeriesMixin::onChartCandlesChanged()
{
	update();
}

const QList<QVariant>& QFCandlesSeriesMixin::dataset() const
{
	return m_chart ? m_chart->candles() : m_dataset;
}

void QFCandlesSeriesMixin::setDataset(const QList<QVariant> &dataset) {}


QFAbstractSeries::QFAbstractSeries(QQuickItem *parent)
	:QQuickItem(parent),
	m_chart(nullptr),
	m_axisX(nullptr),
	m_axisY(nullptr),
	m_plotImage(nullptr)
{
	setFlag(QQuickItem::ItemHasContents, true);
	setAntialiasing(true);
}

void QFAbstractSeries::componentComplete()
{
	if (auto chart = qobject_cast<QFChart*>(parentItem()))
		setChart(chart);
	auto api = window()->rendererInterface()->graphicsApi();
	m_isRhi = QSGRendererInterface::isApiRhiBased(api);
	QQuickItem::componentComplete();
}

void QFAbstractSeries::setChart(QFChart *chart) {
	if (m_chart)
		removeChart();
	m_chart = chart;
	connect(m_chart, &QFChart::plotAreaChanged,
		this, &QFAbstractSeries::onPlotAreaChanged);
	connect(m_chart, &QFChart::candlesChanged,
		this, &QFAbstractSeries::onChartCandlesChanged);
	connectAxesToChart();
}


void QFAbstractSeries::removeChart()
{
	disconnect(m_chart, &QFChart::plotAreaChanged,
		this, &QFAbstractSeries::onPlotAreaChanged);
	disconnect(m_chart, &QFChart::candlesChanged,
		this, &QFAbstractSeries::onChartCandlesChanged);

	if (axisX() && m_chart->axisX() != axisX()) {
		axisX()->disconnectChart(m_chart);
	}
	if (axisY() && m_chart->axisY() != axisY())
		axisY()->disconnectChart(m_chart);

	m_chart = nullptr;
}

void QFAbstractSeries::updateItemHSpacing()
{
	m_itemHSpacing = width() / (m_axisX->max() - m_axisX->min());
}

void QFAbstractSeries::onPlotAreaChanged(const QRectF& plotArea)
{
	setWidth(plotArea.width());
	setHeight(plotArea.height());
	updateItemHSpacing();

	QSize itemSize = size().toSize();
	if (!m_isRhi && (!m_plotImage || itemSize != m_plotImage->size())) {
		delete m_plotImage;
		qreal dpr = window() ? window()->devicePixelRatio() : 1.0;
		m_plotImage = new QImage(itemSize * dpr, QImage::Format_ARGB32);
		m_plotImage->setDevicePixelRatio(dpr);
	}

	if (!m_axisX || !m_axisY)
		return;

	update();
}

void QFAbstractSeries::connectAxesToChart()
{
	if (!m_chart)
		return;
	if (m_axisX)
		m_axisX->connectChart(m_chart);
	if (m_axisY)
		m_axisY->connectChart(m_chart);
}

void QFAbstractSeries::setAxisX(QFAbstractAxis* axis)
{
	if (m_axisX) {
		disconnect(m_axisX, &QFAbstractAxis::rangeChanged,
			this, &QFAbstractSeries::onRangeChanged);
		m_axisX->disconnectChart(m_chart);
	}
	m_axisX = axis;
	connect(m_axisX, &QFAbstractAxis::rangeChanged,
		this, &QFAbstractSeries::onRangeChanged);
	connectAxesToChart();
}

void QFAbstractSeries::setAxisY(QFAbstractAxis* axis)
{
	if (m_axisY)
		m_axisY->disconnectChart(m_chart);
	m_axisY = axis;
	connectAxesToChart();
}

void QFAbstractSeries::setDataset(const QList<QVariant>& dataset) {
	m_dataset = dataset;
	update();
}

void QFAbstractSeries::onRangeChanged(qreal min, qreal max)
{
	// TODO: QObject::sender() check if horizontal or slows down performance?
	updateItemHSpacing();
	update();
}

int QFAbstractSeries::minVisibleIdx() {
	return m_axisX ? qMax((int)round(m_axisX->min()), 0) : 0;
}

int QFAbstractSeries::maxVisibleIdx() {
	if (!m_axisX || dataset().empty())
		return 0;
	return qMin((int)round(m_axisX->max()), dataset().size() - 1);
}

const QRectF QFAbstractSeries::getClipRect()
{
	return QRectF(
		m_axisX->gridLineWidth(),
		m_axisY->gridLineWidth(),
		width() - m_axisX->gridLineWidth() * 2,
		height() - m_axisY->gridLineWidth() * 2
	);
}

QFAbstractAxis* QFAbstractSeries::axisX() { return m_axisX; }

QFAbstractAxis* QFAbstractSeries::axisY() { return m_axisY; }

const QList<QVariant>& QFAbstractSeries::dataset() const { return m_dataset; }

void QFAbstractSeries::onChartCandlesChanged() {};
