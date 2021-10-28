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


#ifndef QFSERIES_H
#define QFSERIES_H

#include <QtQuick/QQuickItem>

class QFAbstractAxis;
class QFChart;


class QFAbstractSeries : public QQuickItem {
	Q_OBJECT
	Q_PROPERTY(QFAbstractAxis *axisX READ axisX WRITE setAxisX)
	Q_PROPERTY(QFAbstractAxis *axisY READ axisY WRITE setAxisY)
	Q_PROPERTY(QList<QVariant> dataset READ dataset WRITE setDataset)
signals:
	void datasetChanged();
public:
	QFAbstractSeries(QQuickItem *parent = 0);
	void componentComplete() override;
	QFAbstractAxis* axisX();
	QFAbstractAxis* axisY();
	void setAxisX(QFAbstractAxis* axis);
	void setAxisY(QFAbstractAxis* axis);
	void setChart(QFChart *chart);
	void removeChart();
	void updateItemHSpacing();
	void onRangeChanged(qreal min, qreal max);
	void onPlotAreaChanged(const QRectF& plotArea);
	virtual const QList<QVariant>& dataset() const;
	virtual void setDataset(const QList<QVariant> &dataset);
	virtual void onChartCandlesChanged();
protected:
	virtual int minVisibleIdx();
	virtual int maxVisibleIdx();
	const QRectF getClipRect();
	qreal m_itemHSpacing;
	QList<QVariant> m_dataset;
	QFChart* m_chart;
	QFAbstractAxis* m_axisX;
	QFAbstractAxis* m_axisY;
	QImage *m_plotImage;
	bool m_isRhi;
private:
	void connectAxesToChart();
};


class QFCandlesSeriesMixin : public QFAbstractSeries {
	Q_OBJECT
	Q_PROPERTY(QColor increasingColor MEMBER m_increasingColor)
	Q_PROPERTY(QColor decreasingColor MEMBER m_decreasingColor)
public:
	QFCandlesSeriesMixin(QQuickItem *parent = 0);
	void onChartCandlesChanged() override;
	const QList<QVariant>& dataset() const override;
	void setDataset(const QList<QVariant> &dataset) override;
protected:
	QColor m_increasingColor;
	QColor m_decreasingColor;
};


class QFCandles : public QFCandlesSeriesMixin {
	Q_OBJECT
	Q_PROPERTY(qreal bodyWidth MEMBER m_bodyWidth)
public:
	QFCandles(QQuickItem *parent = 0);
	QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData *data) override;
private:
	qreal m_bodyWidth;
	qreal m_shadowWidth;
};


class QFVolumes : public QFCandlesSeriesMixin {
	Q_OBJECT
	Q_PROPERTY(qreal barWidth MEMBER m_barWidth)
public:
	QFVolumes(QQuickItem *parent = 0);
	QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData *data) override;
private:
	qreal m_barWidth;
};


class QFLines : public QFAbstractSeries {
	Q_OBJECT
	Q_PROPERTY(QColor lineColor MEMBER m_lineColor)
	Q_PROPERTY(qreal lineWidth MEMBER m_lineWidth)
public:
	QFLines(QQuickItem *parent = 0);
	QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData *data) override;
private:
	qreal startOffset();
	int minVisibleIdx() override;
	int maxVisibleIdx() override;
	QColor m_lineColor;
	qreal m_lineWidth;
};


#endif //QFSERIES_H
