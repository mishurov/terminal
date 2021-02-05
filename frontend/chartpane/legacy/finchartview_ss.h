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


#include <QVariant>
#include <QColor>
#include <QBrush>
#include <QPen>

const QVariantMap chartStyle = {
	{"backgroundVisible", false},
	{"backgroundRoundness", 0},
	{"axesFontSize", 7.5},
};


const QVariantMap priceStyle = {
	{"bodyWidth", 0.75},
	{"increasingColor", QColor(Qt::green)},
	{"decreasingColor", QColor(Qt::red)},
	{"minimumColumnWidth", 0.1},
};


const QVariantMap volumeStyle = {
	{"bodyWidth", 0.9},
	{"increasingColor", QColor(0, 255, 0, 127)},
	{"decreasingColor", QColor(255, 0, 0, 127)},
	{"bodyOutlineVisible", false},
	{"capsVisible", false},
	{"minimumColumnWidth", 0.1},
};

const QBrush labelsBrush(QColor(170, 170, 170));
const QPen linePen(QColor(90, 90, 90), 1);
const QPen gridLinePen(QColor(70, 70, 70), 1);

const QVariantMap axisStyle = {
	{"labelsBrush", labelsBrush},
	{"linePen", linePen},
	{"gridLinePen", gridLinePen},
};


const QVariantMap crossHairStyle = {
	{"lineWidth", 1},
	{"lineColor", QColor(120, 120, 120, 240)},
	{"dashPattern", "3,6"},
	{"axisTextColor", QColor(55, 55, 55)},
	{"axisBgColor", QColor(255, 255, 255)},
};

const QVariantMap priceLineStyle = {
	{"lineWidth", 1},
	{"lineColor", QColor(0, 255, 0, 200)},
	{"lineDColor", QColor(255, 0, 0, 200)},
	{"dashPattern", "1,3"},
	{"axisTextColor", QColor(55, 55, 55)},
	{"axisBgColor", QColor(0, 255, 0)},
	{"axisBgDColor", QColor(255, 0, 0)},
};

const QVariantMap stopLossStyle = {
	{"lineWidth", 1},
	{"lineColor", QColor(255, 0, 0, 240)},
	{"dashPattern", "2,4"},
	{"axisTextColor", QColor(15, 15, 15)},
	{"axisBgColor", QColor(255, 0, 0)},
	{"labelTextColor", QColor(255, 0, 0)},
};
