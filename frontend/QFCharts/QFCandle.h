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

#ifndef QFCANDLE_H
#define QFCANDLE_H

#include <QtCore/QDateTime>
#include <QtGui/QPainter>

struct QFCandle {
	Q_GADGET
	Q_PROPERTY(qreal open MEMBER open)
	Q_PROPERTY(qreal close MEMBER close)
	Q_PROPERTY(qreal high MEMBER high)
	Q_PROPERTY(qreal low MEMBER low)
	Q_PROPERTY(int volume MEMBER volume)
	Q_PROPERTY(QDateTime time MEMBER time)
public:
	qreal open;
	qreal close;
	qreal high;
	qreal low;
	int volume;
	QDateTime time;
};


static const QPainter::RenderHints painterHints = QPainter::Antialiasing
	| QPainter::TextAntialiasing
	| QPainter::SmoothPixmapTransform;


#endif // QFCANDLE_H
