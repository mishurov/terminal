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


#ifndef FINTIMEAXIS_H
#define FINTIMEAXIS_H

#include <private/qbarcategoryaxis_p.h>
#include <private/horizontalaxis_p.h>

class FinChart;
class FinTimeAxis;

class FinTimeAxisX : public HorizontalAxis
{
    Q_OBJECT
public:
	FinTimeAxisX(FinTimeAxis *axis, QGraphicsItem *item);

public Q_SLOTS:
	void handleCategoriesChanged();
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const override;
protected:
	QVector<qreal> calculateLayout() const override;
	void updateGeometry() override;
private:
	FinTimeAxis *m_categoriesAxis;
};


class FinTimeAxisPrivate;

class FinTimeAxis : public QBarCategoryAxis
{
	Q_OBJECT

public:
	FinTimeAxis(QObject *parent = nullptr);

	void setMinTickWidth(int width);
	void setCandleInterval(int inc);
	QList<int> dayIndices() const;
	void appendDayIndex(int tick);
	void clearDayIndices();

	void computeGeometry(const QRectF &gridRect);
	QVector<qreal> layout();
	QStringList labels();

	void setRangeF(qreal min, qreal max);
	qreal minF();
	qreal maxF();

Q_SIGNALS:
    void layoutChanged();

private:
	QList<int> m_visibleIndices;
	QVector<qreal> m_points;
	QStringList m_labels;

	int m_minTickWidth = 70;

	int m_candleInterval = 1;
	QList<int> m_dayIndices;

	Q_DECLARE_PRIVATE(FinTimeAxis)
	Q_DISABLE_COPY(FinTimeAxis)
};


class FinTimeAxisPrivate : public QBarCategoryAxisPrivate
{
	Q_OBJECT
public:
	FinTimeAxisPrivate(FinTimeAxis *q);
	void initializeGraphics(QGraphicsItem* parent) override;
private:
	Q_DECLARE_PUBLIC(FinTimeAxis)
	friend QBarCategoryAxisPrivate;
};

#endif // FINTIMEAXIS_H
