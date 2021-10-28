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


#ifndef QFAXIS_H
#define QFAXIS_H

#include <QtQuick/QQuickPaintedItem>

class QFChart;
class QFAbstractSeries;


class QFAbstractAxis : public QQuickPaintedItem {
	Q_OBJECT
	Q_PROPERTY(Qt::Orientation orientation MEMBER m_orientation)
	Q_PROPERTY(qreal min READ min WRITE setMin NOTIFY rangeChanged)
	Q_PROPERTY(qreal max READ max WRITE setMax NOTIFY rangeChanged)
	Q_PROPERTY(qreal labelsSpace READ labelsSpace WRITE setLabelsSpace NOTIFY labelsSpaceChanged)
	Q_PROPERTY(bool labelsVisible READ labelsVisible WRITE setLabelsVisible NOTIFY labelsVisibleChanged)
	Q_PROPERTY(QFAbstractAxis* parentAxis MEMBER m_parentAxis)
	Q_PROPERTY(QList<qreal> ticks MEMBER m_ticks)
	Q_PROPERTY(QList<QString> labels MEMBER m_labels)
	Q_PROPERTY(qreal minimumTickInterval MEMBER m_minimumTickInterval)
	Q_PROPERTY(qreal gridLineWidth MEMBER m_gridLineWidth)
	Q_PROPERTY(qreal tickSize MEMBER m_tickSize)
	Q_PROPERTY(QColor gridLineColor MEMBER m_gridLineColor)
	Q_PROPERTY(QColor color MEMBER m_color)
	Q_PROPERTY(QColor labelsColor MEMBER m_labelsColor)
	Q_PROPERTY(QFont labelsFont MEMBER m_labelsFont)
signals:
	void rangeChanged(qreal min, qreal max);
	void labelsSpaceChanged(qreal space);
	void labelsVisibleChanged(bool visible);
	void ticksChanged();
public:
	QFAbstractAxis(QQuickItem *parent = 0);
	virtual void paint(QPainter* painter) override;
	void componentComplete() override;
	Q_INVOKABLE virtual void setRange(qreal min, qreal max);
	Q_INVOKABLE virtual void computeTicks();
	qreal labelsSpace();
	void setLabelsSpace(qreal space);
	bool labelsVisible();
	void setLabelsVisible(bool visible);
	qreal gridLineWidth();
	Qt::Orientation orientation();
	qreal min();
	qreal max();
	const QList<qreal>& ticks() const;
	const QList<QString>& labels() const;
	void setMin(qreal min);
	void setMax(qreal max);
	qreal mapValue(qreal v);
	qreal mapPosition(qreal p);
protected:
	virtual void onPlotAreaChanged(const QRectF& plotArea);
	virtual void zoom(const QRectF& rectangle);
	void rangeToZoom();
	void zoomToRange();
	QFAbstractAxis* m_parentAxis;
	QList<qreal> m_ticks;
	QList<QString> m_labels;
	qreal mapRanges(qreal v, qreal inMin, qreal inMax, qreal outMin, qreal outMax);
	Qt::Orientation m_orientation;
	void connectParentAxis();
	qreal m_prevRangeLength;
	qreal m_minimumTickInterval;
	qreal m_min;
	qreal m_max;
	qreal m_gridLineWidth;
	qreal m_tickSize;
	QColor m_gridLineColor;
	QColor m_color;
	QColor m_labelsColor;
	QFont m_labelsFont;
	bool m_labelsVisible;
private:
	void connectChart(QFChart *chart);
	void disconnectChart(QFChart *chart);
	QMetaObject::Connection m_plotAreaChangedConnection;
	QMetaObject::Connection m_chartZoomChangedConnection;

	qreal m_labelsSpace;
	QTransform m_zoom;

	friend class QFAbstractSeries;
};


class QFTimeAxis : public QFAbstractAxis {
	Q_OBJECT
	Q_PROPERTY(QList<qreal> milestoneTicks MEMBER m_milestoneTicks)
public:
	QFTimeAxis(QQuickItem *parent = 0);
	void paint(QPainter* painter) override;
	void setChart(QFChart* chart);
	Q_INVOKABLE void computeTicks() override;
	const QList<qreal>& milestoneTicks() const;
private:
	void computeLabels();
	QFChart* m_chart;
	QList<qreal> m_milestoneTicks;
	int m_prevCandleCount;
};


class QFValueAxis : public QFAbstractAxis {
	Q_OBJECT
	Q_PROPERTY(QString labelFormat MEMBER m_labelFormat)
	Q_PROPERTY(qreal paddingPlotFraction MEMBER m_paddingPlotFraction)
public:
	QFValueAxis(QQuickItem *parent = 0);
	void paint(QPainter* painter) override;
	Q_INVOKABLE void niceZoom(qreal minValue, qreal maxValue);
private:
	qreal m_paddingPlotFraction;
	QString m_labelFormat;
	void computeLabels();
};

#endif // QFAXIS_H
