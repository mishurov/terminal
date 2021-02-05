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


#include "fintimeaxis.h"
#include "finchartview.h"
#include <QtCore/QtMath>
#include <private/chartpresenter_p.h>
#include <private/abstractchartlayout_p.h>


FinTimeAxisX::FinTimeAxisX(FinTimeAxis *axis, QGraphicsItem *item)
	: HorizontalAxis(axis, item, false)
	, m_categoriesAxis(axis)
{
	QObject::connect(m_categoriesAxis, SIGNAL(categoriesChanged()),
		this, SLOT(handleCategoriesChanged()));
		handleCategoriesChanged();
}

void FinTimeAxisX::handleCategoriesChanged()
{
	QGraphicsLayoutItem::updateGeometry();
	if(presenter()) presenter()->layout()->invalidate();
}

QSizeF FinTimeAxisX::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
	Q_UNUSED(constraint)

	QSizeF sh;
	QSizeF base = HorizontalAxis::sizeHint(which, constraint);
	QStringList ticksList = m_categoriesAxis->categories();

	qreal width = 0;
	qreal height = 0;

	switch (which) {
		case Qt::MinimumSize: {
			QRectF boundingRect = ChartPresenter::textBoundingRect(
				axis()->labelsFont(), QStringLiteral("..."),
				axis()->labelsAngle());
			width = boundingRect.width() / 2.0;
			height = boundingRect.height() + labelPadding() + base.height() + 1.0;
			sh = QSizeF(width, height);
			break;
		}
		case Qt::PreferredSize: {
			QRectF boundingRect = ChartPresenter::textBoundingRect(
				axis()->labelsFont(), QStringLiteral("WWWWWW"),
				axis()->labelsAngle());
			width = boundingRect.width() / 2.0;
			height = boundingRect.height() + labelPadding() + base.height() + 1.0;
			sh = QSizeF(width, height);
			break;
		}
		default:
		  break;
	}

	return sh;
}

void FinTimeAxisX::updateGeometry()
{
	setLabels(m_categoriesAxis->labels());
	HorizontalAxis::updateGeometry();
}

QVector<qreal> FinTimeAxisX::calculateLayout() const
{
	const QRectF &gridRect = gridGeometry();
	m_categoriesAxis->computeGeometry(gridRect);
	return m_categoriesAxis->layout();
}


FinTimeAxisPrivate::FinTimeAxisPrivate(FinTimeAxis *q)
	: QBarCategoryAxisPrivate(q) {}

void FinTimeAxisPrivate::initializeGraphics(QGraphicsItem* parent)
{
	Q_Q(FinTimeAxis);
	auto axis = new FinTimeAxisX(q, parent);
	m_item.reset(axis);
	QAbstractAxisPrivate::initializeGraphics(parent);
}

FinTimeAxis::FinTimeAxis(QObject *parent)
	: QBarCategoryAxis(*new FinTimeAxisPrivate(this), parent)
{
}

void FinTimeAxis::setRangeF(qreal min, qreal max)
{
	Q_D(FinTimeAxis);
	d->setRange(min, max);
}

qreal FinTimeAxis::minF()
{
	Q_D(FinTimeAxis);
	return d->min();
}

qreal FinTimeAxis::maxF()
{
	Q_D(FinTimeAxis);
	return d->max();
}

void FinTimeAxis::computeGeometry(const QRectF &gridRect)
{
	m_visibleIndices.clear();
	m_points.clear();
	m_labels.clear();

	QPair<qreal, qreal> range;
	range.first = minF();
	range.second = maxF();

	if (qIsNaN(range.first) || qIsNaN(range.second)
	    || range.second - range.first < 2) {
		m_points << gridRect.left() << gridRect.right();
		m_labels << "0" << "1";
		return;
	}

	qreal maxTickNum = gridRect.width() / m_minTickWidth;

	int vmin = qRound(range.first);
	int vmax = qRound(range.second);
	int vnum = vmax - vmin;

	qreal bucketSize = vnum / maxTickNum * m_candleInterval;

	// 60 mins, 30 mins, 15 mins, 10 mins, 5 mins
	int roundBucket = 5;
	if (bucketSize < 60) {
		if (bucketSize > 30) {
			roundBucket = 60;
		} else if (bucketSize > 15) {
			roundBucket = 30;
		} else if (bucketSize > 10) {
			roundBucket = 15;
		} else if (bucketSize > 5) {
			roundBucket = 10;
		}
	} else {
		roundBucket = qCeil(bucketSize / 60) * 60;
	}

	int last = categories().count() - 1;
	if (last >= vmin && last <= vmax)
		m_visibleIndices.append(last);

	int numSkip = roundBucket / m_candleInterval;

	last = m_visibleIndices.count() ? last - numSkip : qMin(vmax, last);

	vmin = qMax(vmin, 0);

	for (int i = last; i >= vmin; i--) {
		auto ts = categories().at(i);
		int mins = QDateTime::fromString(ts, Qt::ISODate).toString("m").toInt();

		if ((roundBucket < 60 && !(mins % roundBucket)) ||
		    (roundBucket >= 60 && mins == 0)) {
			last = i;
			m_visibleIndices.append(last);
			break;
		}
	}

	QList<int> dts;
	dts.append(m_dayIndices);

	for (int i = last; i >= vmin; i -= numSkip) {
		for (auto dt : dts) {
			if (dt >= i) {
				if (dt > i) {
					m_visibleIndices.removeLast();
					i = dt;
				}
				// TODO probably needs again the single candle iteration
				// until a next round date found
				dts.removeOne(dt);
				break;
			}
		}

		m_visibleIndices.append(i);
	}

	qreal factor = gridRect.width() / (range.second - range.first);

	for (int i = m_visibleIndices.size() - 1; i >= 0; i--) {
		int index = m_visibleIndices.at(i);
		qreal dist = (qreal)index - range.first;
		m_points.append(gridRect.left() + dist * factor);

		QString fmt = m_dayIndices.contains(index) ? "MMM d" : "hh:mm";
		auto cat = QDateTime::fromString(
			categories().at(index), Qt::ISODate).toString(fmt);
		m_labels.append(cat);
	}

	emit layoutChanged();
}

QVector<qreal> FinTimeAxis::layout() { return m_points; }
QStringList FinTimeAxis::labels() { return m_labels; }

void FinTimeAxis::setMinTickWidth(int width) { m_minTickWidth = width; }
void FinTimeAxis::setCandleInterval(int cint) { m_candleInterval = cint; }

QList<int> FinTimeAxis::dayIndices() const { return m_dayIndices; }
void FinTimeAxis::appendDayIndex(int idx) { m_dayIndices.append(idx); }
void FinTimeAxis::clearDayIndices() { m_dayIndices.clear(); }
