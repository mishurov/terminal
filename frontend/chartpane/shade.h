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


#ifndef SHADE_H
#define SHADE_H

#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>
#include <QtCore/QDateTime>

QT_CHARTS_USE_NAMESPACE

class FinChart;


class Shade : public QObject, public QGraphicsRectItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

	Q_PROPERTY(QColor color READ color WRITE setColor)

public:
	Shade(FinChart *parent);

	QColor color();
	void setColor(QColor color);

	void setPriceRange(qreal min, qreal max);
	void setTimeRange(QDateTime min, QDateTime max);

	QPair<QDateTime, QDateTime> timeRange();

private:
	void rangeChanged();
	void priceSeriesAdded();

	void updateColor();
	void updateRect();
	void updateTimeRangeF();

	qreal mapX(qreal x);
	qreal mapY(qreal y);

	Qt::Orientation m_orientation;
	FinChart *m_chart;

	QPair<qreal, qreal> m_priceRange = { qQNaN(), qQNaN() };

	QPair<QDateTime, QDateTime> m_timeRange;
	QPair<qreal, qreal> m_timeRangeF = { qQNaN(), qQNaN() };

	QColor m_color = QColor(100, 100, 100);
};

#endif // SHADE_H
