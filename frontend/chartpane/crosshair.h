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


#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>
#include <QtCore/QDateTime>
#include <QtGui/QFont>
#include <QtGui/QPen>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE


QT_CHARTS_USE_NAMESPACE

class FinChart;
class MovingLabel;
class PlotLabel;

class CrossHair : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

	Q_PROPERTY(qreal lineWidth READ lineWidth WRITE setLineWidth)
	Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor)
	Q_PROPERTY(QColor lineDColor READ lineDColor WRITE setLineDColor)
	Q_PROPERTY(QString dashPattern READ dashPattern WRITE setDashPattern)
	Q_PROPERTY(QColor axisTextColor READ axisTextColor WRITE setAxisTextColor)
	Q_PROPERTY(QColor axisBgColor READ axisBgColor WRITE setAxisBgColor)
	Q_PROPERTY(QColor axisBgDColor READ axisBgDColor WRITE setAxisBgDColor)
	Q_PROPERTY(QColor labelTextColor READ labelTextColor WRITE setLabelTextColor)

public:
	CrossHair(FinChart *parent, Qt::Orientation orientation=Qt::Horizontal);

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void setPos(const QPointF &position);
	void setZValue(qreal z);
	void setVisible(bool visible);
	void setText(const QString &text);

	QPointF value() const;
	void setValue(const QPointF &value);
	void setDraggable(bool draggable);
	void setFollowRange(bool followRange);

	qreal lineWidth();
	void setLineWidth(qreal width);
	QColor lineColor();
	void setLineColor(QColor color);
	QColor lineDColor();
	void setLineDColor(QColor color);
	QString dashPattern();
	void setDashPattern(QString pattern);

	QColor axisTextColor();
	void setAxisTextColor(QColor color);
	QColor axisBgColor();
	void setAxisBgColor(QColor color);
	QColor axisBgDColor();
	void setAxisBgDColor(QColor color);
	QColor labelTextColor();
	void setLabelTextColor(QColor color);

	void setIsUp(bool isUp);

	void setSibling(CrossHair* sibling);

	bool isDragged();
	void setDragYLimit(qreal limit);

signals:
	void dragValueChanged(QPointF value);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
private:
	void plotAreaChanged(const QRectF &plotArea);
	void rangeChanged();
	void updatePen();

	void updateValueFromPos();
	void updatePosFromValue();
	void updateTextFromValue();
	void updatePos();
	void discretizeTime();

	qreal m_lineWidth = 1;
	QColor m_lineColor = QColor(120, 120, 120, 240);
	QColor m_lineDColor = QColor(120, 120, 120, 240);
	QVector<qreal> m_dashPattern = {3, 6};

	bool m_isUp = true;
	qreal m_draggableBorder = 1.1;
	bool m_draggable = true;
	bool m_followRange = true;
	QPen m_pen;
	QPen m_penD;

	FinChart *m_chart;
	Qt::Orientation m_orientation;
	QPointF m_fullPos;

	MovingLabel *m_movingLabel;
	PlotLabel *m_plotLabel;

	QPointF m_value;
	QPointF m_mousePressValue;
	bool m_isDragged = false;
	// as the price axis is always positive
	// negative value mean the limit direction
	// for example -10 means no lower than 10
	// -10 means no upper than 10
	qreal m_dragYLimit;

	CrossHair* m_sibling = nullptr;
};


class MovingLabel : public QGraphicsItem
{
public:
	MovingLabel(FinChart *parent, Qt::Orientation orientation=Qt::Horizontal);

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void setPos(const QPointF &position);
	void setText(const QString &text);

	QColor textColor();
	void setTextColor(QColor color);
	QColor bgColor();
	void setBgColor(QColor color);
	QColor bgDColor();
	void setBgDColor(QColor color);

	void setIsUp(bool isUp);
private:
	FinChart *m_chart;
	Qt::Orientation m_orientation;

	QString m_text;
	QRectF m_textRect;
	QRectF m_rect;
	QFont m_font;

	bool m_isUp = true;

	QColor m_textColor = QColor(55, 55, 55);
	QColor m_bgColor = QColor(255, 255, 255);
	QColor m_bgDColor = QColor(255, 255, 255);

	QPointF m_fullPos;
};


class PlotLabel : public QGraphicsSimpleTextItem
{
public:
	PlotLabel(FinChart *parent, Qt::Orientation orientation=Qt::Horizontal);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	void setPos(const QPointF &position);

	QColor textColor();
	void setTextColor(QColor color);
private:
	FinChart *m_chart;
	Qt::Orientation m_orientation;
	QPointF m_fullPos;

	QColor m_textColor = QColor(55, 55, 55);
};


#endif // CROSSHAIR_H
