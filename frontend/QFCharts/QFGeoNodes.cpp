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


#include "QFGeoNodes.h"


QFBaseFlatGeoNode::QFBaseFlatGeoNode()
	: m_geometry(QSGGeometry::defaultAttributes_Point2D(), 0)
{
	setGeometry(&m_geometry);
	m_geometry.setDrawingMode(QSGGeometry::DrawTriangles);
	setMaterial(&m_material);
	m_material.setColor(Qt::gray);
}

void QFBaseFlatGeoNode::setColor(const QColor& color)
{
	if (m_material.color() == color)
		return;
	m_material.setColor(color);
	if (color.alpha() == 255) {
		setOpaqueMaterial(&m_material);
		setMaterial(nullptr);
	} else {
		setMaterial(&m_material);
		setOpaqueMaterial(nullptr);
	}
	markDirty(QSGNode::DirtyMaterial);
}


void QFRectsNode::setRects(const QList<QRectF>& rects)
{
	QSGGeometry *g = geometry();

	if (rects.empty()) {
		g->allocate(1);
		markDirty(QSGNode::DirtyGeometry);
		return;
	}

	g->allocate(rects.size() * 6);
	QSGGeometry::Point2D *v = g->vertexDataAsPoint2D();
	for (int i = 0; i < rects.size(); i++) {
		const QRectF& r = rects.at(i);
		v[i * 6].set(r.x(), r.y());
		v[i * 6 + 1].set(r.x(), r.y() + r.height());
		v[i * 6 + 2].set(r.x() + r.width(), r.y() + r.height());
		v[i * 6 + 3].set(r.x() + r.width(), r.y() + r.height());
		v[i * 6 + 4].set(r.x() + r.width(), r.y());
		v[i * 6 + 5].set(r.x(), r.y());
	}

	markDirty(QSGNode::DirtyGeometry);
}


QFPolyLineNode::QFPolyLineNode()
	: m_lineWidth(1)
{
	geometry()->setDrawingMode(QSGGeometry::DrawTriangleStrip);
}

void QFPolyLineNode::setLineWidth(float lineWidth)
{
	m_lineWidth = lineWidth;
}

void QFPolyLineNode::setPoints(const QList<QPointF>& points)
{
	QSGGeometry *g = geometry();

	if (points.size() < 2) {
		g->allocate(1);
		markDirty(QSGNode::DirtyGeometry);
		return;
	}

	g->allocate(points.size() * 4);
	QSGGeometry::Point2D *v = g->vertexDataAsPoint2D();
	float hw = m_lineWidth / 2;

	for (int i = 0; i < points.size(); i++) {
		const QPointF& pc = points.at(i);
		QPointF pp;
		QPointF pn;
		if (i == 0) {
			pn = points.at(i + 1);
			pp = pc + QVector2D(pc - pn).normalized().toPointF();
		} else if (i == points.size() - 1) {
			pp = points.at(i - 1);
			pn = pc + QVector2D(pc - pp).normalized().toPointF();
		} else {
			pp = points.at(i - 1);
			pn = points.at(i + 1);
		}
		auto ab = QVector2D(pc - pp).normalized();
		auto bc = QVector2D(pn - pc).normalized();
		auto normalA = QVector2D(-ab.y(), ab.x());
		auto normalB = QVector2D(-bc.y(), bc.x());
		auto a = hw * normalA;
		auto b = hw * normalB;
		v[i * 4].set(pc.x() - a.x(), pc.y() - a.y());
		v[i * 4 + 1].set(pc.x() + a.x(), pc.y() + a.y());
		v[i * 4 + 2].set(pc.x() - b.x(), pc.y() - b.y());
		v[i * 4 + 3].set(pc.x() + b.x(), pc.y() + b.y());
	}

	markDirty(QSGNode::DirtyGeometry);
}
