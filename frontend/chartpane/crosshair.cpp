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


#include "crosshair.h"
#include "finchartview.h"
#include "fintimeaxis.h"

#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtGui/QMouseEvent>
#include <QtCharts/QChart>
#include <QtGui/QCursor>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>

#include <QtCore/QDebug>


CrossHair::CrossHair(FinChart *chart, Qt::Orientation orientation):
	QObject(),
	QGraphicsItem(chart),
	m_chart(chart),
	m_orientation(orientation)
{
	updatePen();

	m_plotLabel = new PlotLabel(m_chart, orientation);
	m_plotLabel->setVisible(false);

	m_movingLabel = new MovingLabel(m_chart, orientation);

	setDraggable(m_draggable);

	connect(m_chart, &QChart::plotAreaChanged, this, &CrossHair::plotAreaChanged);

	auto *axisTime = m_chart->timeAxis();
	auto *axisPrice = m_chart->priceAxis();
	connect(axisTime, &FinTimeAxis::rangeChanged, this, &CrossHair::rangeChanged);
	connect(axisPrice, &QValueAxis::rangeChanged, this, &CrossHair::rangeChanged);

	setZValue(4);
}

void CrossHair::rangeChanged() {
	if (!m_followRange)
		return;

	updatePosFromValue();
	updatePos();
}

void CrossHair::plotAreaChanged(const QRectF &plotArea)
{
	if (m_followRange)
		updatePosFromValue();

	// TODO: possible conflicts followRange / draggable

	QPointF pos = plotArea.center();
	m_orientation == Qt::Horizontal ?
		pos.setY(m_fullPos.y()) : pos.setX(m_fullPos.x());

	setPos(pos);
}

QPointF CrossHair::value() const
{
	return m_value;
}

void CrossHair::setValue(const QPointF &value)
{
	m_value = value;
	updatePosFromValue();
	updatePos();
	updateTextFromValue();
}

void CrossHair::setDraggable(bool draggable)
{
	m_draggable = draggable;
	setAcceptHoverEvents(m_draggable);
}

void CrossHair::setFollowRange(bool followRange)
{
	m_followRange = followRange;
}

void CrossHair::updateValueFromPos()
{
	m_value = m_chart->mapToValue(m_fullPos, m_chart->priceSeries());
}

void CrossHair::updatePosFromValue()
{
	auto mappedPos = m_chart->mapToPosition(m_value, m_chart->priceSeries());

	QPointF pos = m_chart->plotArea().center();
	m_orientation == Qt::Horizontal ?
		pos.setY(mappedPos.y()) : pos.setX(mappedPos.x());

	m_fullPos = pos;
}

void CrossHair::updateTextFromValue()
{
	if (m_orientation == Qt::Horizontal) {
		m_movingLabel->setText(QString::number(m_value.y(), 'f', 2));
	} else {
		int count = m_chart->timeAxis()->count();
		if (!count)
			return;
		int index = qRound(m_value.x());
		index = qMin(qMax(0, index), count - 1);
		auto cat = QDateTime::fromString(
			m_chart->timeAxis()->at(index), Qt::ISODate).toString("hh:mm");
		m_movingLabel->setText(cat);
	}
}

void CrossHair::updatePos()
{
	m_movingLabel->setPos(m_fullPos);
	m_plotLabel->setPos(m_fullPos);
	// prepareGeometryChange();
	if (m_orientation == Qt::Horizontal)
		QGraphicsItem::setPos(x(), m_fullPos.y());
	else
		QGraphicsItem::setPos(m_fullPos.x(), y());
}

void CrossHair::setPos(const QPointF &position)
{
	m_fullPos = position;
	updateValueFromPos();
	discretizeTime();
	updatePos();
	updateTextFromValue();
}

void CrossHair::discretizeTime()
{
	if (m_orientation != Qt::Vertical || !m_chart->timeAxis()->count())
		return;
	m_value.setX(qRound(m_value.x()));
	m_fullPos = m_chart->mapToPosition(m_value, m_chart->priceSeries());
}

void CrossHair::setZValue(qreal z)
{
	QGraphicsItem::setZValue(z);
	m_movingLabel->setZValue(z);
	m_plotLabel->setZValue(z);
}

void CrossHair::setVisible(bool visible)
{
	QGraphicsItem::setVisible(visible);
	m_movingLabel->setVisible(visible);

	if (!m_plotLabel->text().isEmpty())
		m_plotLabel->setVisible(visible);
}

QRectF CrossHair::boundingRect() const
{
	auto area = m_chart->plotArea();

	//if (!area.contains(m_fullPos))
	//	return QRectF();

	qreal x1, y1, x2, y2;

	qreal halfWidth = m_lineWidth + m_draggableBorder;

	if (m_orientation == Qt::Horizontal) {
		x1 = area.left();
		y1 = -halfWidth;
		x2 = area.right();
		y2 = halfWidth;
	} else {
		x1 = -halfWidth;
		y1 = area.top();
		x2 = halfWidth;
		y2 = area.bottom();
	}

	return QRectF(QPointF(x1, y1), QPointF(x2, y2));
}

void CrossHair::updatePen() {
	m_pen.setColor(m_lineColor);
	m_pen.setWidth(m_lineWidth);
	m_pen.setDashPattern(m_dashPattern);

	m_penD.setColor(m_lineDColor);
	m_penD.setWidth(m_lineWidth);
	m_penD.setDashPattern(m_dashPattern);
}

void CrossHair::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	auto area = m_chart->plotArea();

	if (!area.contains(m_fullPos))
		return;

	painter->setPen(m_isUp ? m_pen : m_penD);

	qreal x1, y1, x2, y2;

	if (m_orientation == Qt::Horizontal) {
		y1 = y2 = 0;
		x1 = area.left();
		x2 = area.right();
	} else {
		x1 = x2 = 0;
		y1 = area.top();
		y2 = area.bottom();
	}

	painter->drawLine(x1, y1, x2, y2);
}

void CrossHair::setText(const QString &text)
{
	if (text.isEmpty()) {
		m_plotLabel->setVisible(false);
		return;
	}
	m_plotLabel->setVisible(true);
	m_plotLabel->setText(text);
}

void CrossHair::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	if (!m_draggable)
		return;

	Qt::CursorShape shape = m_orientation == Qt::Horizontal ?
		Qt::SplitVCursor : Qt::SplitHCursor;
	m_chart->setCursor(shape);
}

void CrossHair::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	if (m_draggable)
		m_chart->setCursor(Qt::ArrowCursor);
}

void CrossHair::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (!m_draggable)
		return;
	
	if (m_sibling != nullptr)
		m_sibling->setVisible(false);

	m_isDragged = true;
	m_mousePressValue = m_value;
	event->setAccepted(true);
}

void CrossHair::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_draggable && event->buttons() & Qt::LeftButton){
		QPointF pos = mapToParent(event->pos());
		auto value = m_chart->mapToValue(pos, m_chart->priceSeries());
		bool up = m_dragYLimit > 0;
		if ((up && value.y() < m_dragYLimit) || (!up && value.y() > qAbs(m_dragYLimit)))
			setPos(pos);
		event->setAccepted(true);
	} else {
		event->setAccepted(false);
	}
}

void CrossHair::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (!m_draggable)
		return;

	if (m_sibling != nullptr)
		m_sibling->setVisible(true);

	m_isDragged = false;

	if ((m_orientation == Qt::Horizontal && m_mousePressValue.y() != m_value.y())
	    || (m_orientation == Qt::Vertical && m_mousePressValue.x() != m_value.x()))
		emit dragValueChanged(m_value);

	event->setAccepted(true);
}

bool CrossHair::isDragged() { return m_isDragged; }
void CrossHair::setDragYLimit(qreal limit) { m_dragYLimit = limit; };

void CrossHair::setSibling(CrossHair* sibling) { m_sibling = sibling; }

qreal CrossHair::lineWidth() { return m_lineWidth; }
void CrossHair::setLineWidth(qreal width) { m_lineWidth = width; updatePen(); }

QColor CrossHair::lineColor() { return m_lineColor; }
void CrossHair::setLineColor(QColor color) { m_lineColor = color; updatePen(); }

QColor CrossHair::lineDColor() { return m_lineDColor; }
void CrossHair::setLineDColor(QColor color) { m_lineDColor = color; updatePen(); }

QColor CrossHair::axisTextColor() { return m_movingLabel->textColor(); }
void CrossHair::setAxisTextColor(QColor c) { m_movingLabel->setTextColor(c); }

void CrossHair::setIsUp(bool isUp) { m_isUp = isUp; m_movingLabel->setIsUp(isUp); }

QString CrossHair::dashPattern()
{
	QStringList slist;
	for (auto r : m_dashPattern)
		slist.append(QString().setNum(r));
	return slist.join(",");
}
void CrossHair::setDashPattern(QString pattern) {
	auto slist = pattern.split(",");
	QVector<qreal> dashPattern;
	for (auto s : slist)
		dashPattern.append(s.toDouble());
	m_dashPattern = dashPattern;
	updatePen();
}

QColor CrossHair::axisBgColor() { return m_movingLabel->bgColor(); }
void CrossHair::setAxisBgColor(QColor c) { m_movingLabel->setBgColor(c); }

QColor CrossHair::axisBgDColor() { return m_movingLabel->bgDColor(); }
void CrossHair::setAxisBgDColor(QColor c) { m_movingLabel->setBgDColor(c); }

QColor CrossHair::labelTextColor() { return m_plotLabel->textColor(); }
void CrossHair::setLabelTextColor(QColor c) { m_plotLabel->setTextColor(c); }


MovingLabel::MovingLabel(FinChart *chart, Qt::Orientation orientation):
	QGraphicsItem(chart),
	m_chart(chart),
	m_orientation(orientation)
{
}

QRectF MovingLabel::boundingRect() const
{
	return m_rect;
}

void MovingLabel::setPos(const QPointF &position) {
	m_fullPos = position;

	auto area = m_chart->plotArea();
	// prepareGeometryChange();
	if (m_orientation == Qt::Horizontal)
		QGraphicsItem::setPos(area.right(), m_fullPos.y());
	else
		QGraphicsItem::setPos(m_fullPos.x(), area.bottom());
}

void MovingLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	auto area = m_chart->plotArea();

	if (!area.contains(m_fullPos))
		return;
	
	if (m_isUp) {
		painter->setPen(m_bgColor);
		painter->setBrush(m_bgColor);
	} else {
		painter->setPen(m_bgDColor);
		painter->setBrush(m_bgDColor);
	}
	painter->drawRect(m_rect);

	painter->setPen(m_textColor);
	painter->setFont(m_font);
	painter->drawText(m_textRect, m_text);
}

void MovingLabel::setText(const QString &text)
{
	m_text = text;
	m_font.setPixelSize(9);
	QFontMetrics metrics(m_font);

	int alignment;
	if (m_orientation == Qt::Horizontal) {
		alignment = Qt::AlignLeft | Qt::AlignVCenter;
	} else {
		alignment = Qt::AlignTop | Qt::AlignHCenter;
	}

	m_textRect = metrics.boundingRect(QRect(), alignment, m_text);
	
	qreal vPadding = 5;
	qreal hPadding = 5;

	if (m_orientation == Qt::Horizontal) {
		m_textRect.translate(hPadding, 0);
		vPadding = 1;
	} else {
		m_textRect.translate(0, vPadding);
		hPadding = 3;
	}
	prepareGeometryChange();
	m_rect = m_textRect.adjusted(-hPadding, -vPadding, hPadding, vPadding);
}

QColor MovingLabel::textColor() { return m_textColor; }
void MovingLabel::setTextColor(QColor color) { m_textColor = color; }

QColor MovingLabel::bgColor() { return m_bgColor; }
void MovingLabel::setBgColor(QColor color) { m_bgColor = color; }

QColor MovingLabel::bgDColor() { return m_bgDColor; }
void MovingLabel::setBgDColor(QColor color) { m_bgDColor = color; }

void MovingLabel::setIsUp(bool isUp) { m_isUp = isUp; }


PlotLabel::PlotLabel(FinChart *chart, Qt::Orientation orientation):
	QGraphicsSimpleTextItem(chart),
	m_chart(chart),
	m_orientation(orientation)
{
	setBrush(QColor(255, 255, 255));
	auto font = this->font();
	font.setPixelSize(9);
	setFont(font);
}

void PlotLabel::setPos(const QPointF &position) {
	m_fullPos = position;
	auto area = m_chart->plotArea();

	qreal borderMargin = 10;
	qreal axisHMargin = 3;
	qreal axisVMargin = 6;
	auto bbox = boundingRect();

	qreal x, y;
	if (m_orientation == Qt::Horizontal) {
		x = area.right() - bbox.width() - borderMargin;
		if (m_fullPos.y() - area.top() < axisHMargin * 2 + bbox.height())
			y = m_fullPos.y() + axisHMargin;
		else
			y = m_fullPos.y() - axisHMargin - bbox.height();
	} else {
		y = area.top() + borderMargin;

		if (area.right() - m_fullPos.x() < axisVMargin * 2 + bbox.width())
			x = m_fullPos.x() - borderMargin - bbox.height();
		else
			x = m_fullPos.x() + borderMargin;
	}
	// prepareGeometryChange();
	QGraphicsSimpleTextItem::setPos(x, y);
}

void PlotLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	auto area = m_chart->plotArea();

	if (!area.contains(m_fullPos))
		return;

	QGraphicsSimpleTextItem::paint(painter, option, widget);
}

QColor PlotLabel::textColor() { return m_textColor; }
void PlotLabel::setTextColor(QColor color) { m_textColor = color; setBrush(m_textColor); }
