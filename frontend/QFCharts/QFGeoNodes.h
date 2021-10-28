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


#ifndef QFGEONODES_H
#define QFGEONODES_H

#include <QtQuick/QSGFlatColorMaterial>


class QFBaseFlatGeoNode : public QSGGeometryNode {
public:
	QFBaseFlatGeoNode();
	void setColor(const QColor& color);
protected:
	QSGGeometry m_geometry;
private:
	QSGFlatColorMaterial m_material;
	void *reserved;
};


class QFRectsNode : public QFBaseFlatGeoNode {
public:
	void setRects(const QList<QRectF>& rects);
};


class QFPolyLineNode : public QFBaseFlatGeoNode {
public:
	QFPolyLineNode();
	void setPoints(const QList<QPointF>& points);
	void setLineWidth(float lineWidth);
private:
	float m_lineWidth;
};


#endif //QFGEONODES_H
