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

#include "QFCrosshair.h"

#include "QFCandle.h"
#include "QFGeoNodes.h"

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtGui/QPainter>


QFCrosshair::QFCrosshair(QQuickItem *parent)
	:QQuickItem(parent),
	m_strokeColor(Qt::gray),
	m_strokeWidth(1),
	m_orientation(Qt::Horizontal),
	m_image(nullptr)
{
	setAntialiasing(true);
	setFlag(QQuickItem::ItemHasContents, true);
	connect(this, &QFCrosshair::linePosChanged, this, &QFCrosshair::update);
}

QSGNode* QFCrosshair::updatePaintNode(QSGNode *node, UpdatePaintNodeData *data)
{
	QSGNode *n = node;
	QSGSimpleTextureNode *tn = nullptr;
	QFRectsNode *dashes = nullptr;

	if (!n) {
		n = new QSGNode();
		dashes = new QFRectsNode();
		n->appendChildNode(dashes);
		if (!m_isRhi) {
			tn = new QSGSimpleTextureNode();
			n->appendChildNode(tn);
		}
	} else {
		dashes = static_cast<QFRectsNode *>(n->childAtIndex(0));
		if (!m_isRhi) {
			tn = static_cast<QSGSimpleTextureNode *>(n->childAtIndex(1));
		}
	}

	QList<QRectF> dashesRects;
	qreal strokeHalfWidth = m_strokeWidth / 2;

	if (m_dashPattern.empty()) {
		QRectF d = m_orientation == Qt::Horizontal
			? QRectF(0,  m_linePos.y() - strokeHalfWidth, width(), m_strokeWidth)
			: QRectF(m_linePos.x() - strokeHalfWidth, 0, m_strokeWidth, height());
		dashesRects.push_back(d);
	} else {
		qreal endPos = m_orientation == Qt::Horizontal ? width() : height();
		for (qreal pos = 0; pos < endPos;) {
			for (int i = 0; i < m_dashPattern.size() && pos < endPos; i++) {
				qreal len = m_dashPattern.at(i);
				pos += len;
				if (i % 2)
					continue;
				QRectF d = m_orientation == Qt::Horizontal
					? QRectF(pos - len,  m_linePos.y() - strokeHalfWidth, len, m_strokeWidth)
					: QRectF(m_linePos.x() - strokeHalfWidth, pos - len, m_strokeWidth, len);
				dashesRects.push_back(d);
			}
		}
	}

	dashes->setColor(m_strokeColor);
	dashes->setRects(dashesRects);

	if (m_isRhi || !m_image)
		return n;

	m_image->fill(Qt::transparent);
	QPainter painter(m_image);
	painter.setRenderHints(painterHints);
	painter.setPen(Qt::NoPen);
	painter.setBrush(m_strokeColor);
	painter.drawRects(dashesRects);

	QSGTexture *texture = window()->createTextureFromImage(*m_image);
	tn->setTexture(texture);
	tn->setRect(0, 0, width(), height());
	return n;
}

void QFCrosshair::componentComplete()
{
	auto api = window()->rendererInterface()->graphicsApi();
	m_isRhi = QSGRendererInterface::isApiRhiBased(api);
	QQuickItem::componentComplete();
}


void QFCrosshair::geometryChange(const QRectF &newGeometry,
	const QRectF &oldGeometry)
{
	if (newGeometry.isValid() && newGeometry.width() > 0 && newGeometry.height() > 0) {
		QSize itemSize = newGeometry.size().toSize();
		if (!m_isRhi && (!m_image || itemSize != m_image->size())) {
			delete m_image;
			qreal dpr = window() ? window()->devicePixelRatio() : 1.0;
			m_image = new QImage(itemSize * dpr, QImage::Format_ARGB32);
			m_image->setDevicePixelRatio(dpr);
		}
	}
	QQuickItem::geometryChange(newGeometry, oldGeometry);
}
