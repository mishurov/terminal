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


#ifndef QFCHART_H
#define QFCHART_H

#include <QtQuick/QQuickItem>

class QFAbstractSeries;
class QFAbstractAxis;


class QFChart : public QQuickItem {
	Q_OBJECT
	Q_PROPERTY(QList<QVariant> candles READ candles WRITE setCandles NOTIFY candlesChanged)
	Q_PROPERTY(int selectedInterval READ selectedInterval WRITE setSelectedInterval)
	Q_PROPERTY(QRectF plotArea READ plotArea NOTIFY plotAreaChanged)
signals:
	void plotAreaChanged(const QRectF& plotArea);
	void zoomChanged(const QRectF& rectangle);
	void candlesChanged();
public:
	QFChart(QQuickItem *parent = 0);
	// TODO background color
	//QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData *data) override;
	void componentComplete() override;
	void geometryChange(const QRectF &newGeometry,
		const QRectF &oldGeometry) override;
	void onLabelsStateChanged();
	void setPlotAreaSize(qreal chartWidth, qreal chartHeight);
	Q_INVOKABLE void scrollLeft(qreal pixels);
	Q_INVOKABLE void zoomIn(QRectF &rectangle);
	Q_INVOKABLE QPointF mapToPosition(QPointF v);
	Q_INVOKABLE QPointF mapToValue(QPointF p);
	QFAbstractAxis* axisX();
	QFAbstractAxis* axisY();
	int selectedInterval();
	void setSelectedInterval(int interval);
	const QList<QVariant>& candles() const;
	const QRectF& plotArea() const;
	void setCandles(const QList<QVariant>& candles);
	Q_INVOKABLE void addSeries(QFAbstractSeries *series);
	Q_INVOKABLE void removeSeries(QFAbstractSeries *series);
private:
	QFAbstractAxis* m_axisX;
	QFAbstractAxis* m_axisY;
	QRectF m_plotArea;
	QList<QVariant> m_candles;
	QList<QFAbstractSeries*> m_series;
	int m_selectedInterval;
};


#endif // QFCHART_H
