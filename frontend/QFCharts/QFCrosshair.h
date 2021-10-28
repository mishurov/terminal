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


#ifndef QFCROSSHAIR_H
#define QFCROSSHAIR_H

#include <QtQuick/QQuickItem>

// TODO: move everything from QML in here

class QFCrosshair : public QQuickItem {
	Q_OBJECT
	Q_PROPERTY(Qt::Orientation orientation MEMBER m_orientation)
	Q_PROPERTY(QPointF linePos MEMBER m_linePos NOTIFY linePosChanged)
	Q_PROPERTY(QColor strokeColor MEMBER m_strokeColor)
	Q_PROPERTY(qreal strokeWidth MEMBER m_strokeWidth)
	Q_PROPERTY(QList<qreal> dashPattern MEMBER m_dashPattern)
signals:
	void linePosChanged(QPointF pos);
public:
	QFCrosshair(QQuickItem *parent = 0);
	QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData *data) override;
	void componentComplete() override;
	void geometryChange(const QRectF &newGeometry,
		const QRectF &oldGeometry) override;
private:
	QPointF m_linePos;
	QColor m_strokeColor;
	qreal m_strokeWidth;
	QList<qreal> m_dashPattern;
	Qt::Orientation m_orientation;
	QImage *m_image;
	bool m_isRhi;
};



#endif // QFCROSSHAIR_H
