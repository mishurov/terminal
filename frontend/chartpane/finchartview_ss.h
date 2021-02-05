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


#include <QtCore/QVariant>
#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QPen>

const QVariantMap chartStyle = {
	{"backgroundVisible", false},
	{"backgroundRoundness", 0},
	{"axesFontSize", 9},
};


const QVariantMap priceStyle = {
	{"bodyWidth", 0.75},
	{"increasingColor", QColor(0, 230, 0)},
	{"decreasingColor", QColor(230, 0, 0)},
	{"minimumColumnWidth", 0.1},
};


const QVariantMap volumeStyle = {
	{"barWidth", 0.9},
	{"labelsVisible", false},
	{"increasingColor", QColor(0, 255, 0, 127)},
	{"decreasingColor", QColor(255, 0, 0, 127)},
	{"capsVisible", false},
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
	{"lineColor", QColor(10, 235, 10, 200)},
	{"lineDColor", QColor(235, 10, 10, 200)},
	{"dashPattern", "1,3"},
	{"axisTextColor", QColor(55, 55, 55)},
	{"axisBgColor", QColor(10, 210, 10)},
	{"axisBgDColor", QColor(210, 10, 10)},
};

const QVariantMap stopLossStyle = {
	{"lineWidth", 1},
	{"lineColor", QColor(255, 30, 30)},
	{"dashPattern", "2,4"},
	{"axisTextColor", QColor(10, 10, 10)},
	{"axisBgColor", QColor(235, 0, 0)},
	{"labelTextColor", QColor(255, 10, 10)},
};

const QVariantMap takeProfitStyle = {
	{"lineWidth", 1},
	{"lineColor", QColor(0, 255, 0, 240)},
	{"dashPattern", "2,4"},
	{"axisTextColor", QColor(15, 15, 15)},
	{"axisBgColor", QColor(0, 255, 0)},
	{"labelTextColor", QColor(0, 255, 0)},
};

const QVariantMap preShadeStyle = {
	{"color", QColor(47, 47, 47)},
};

const QVariantMap postShadeStyle = {
	{"color", QColor(42, 42, 42)},
};
