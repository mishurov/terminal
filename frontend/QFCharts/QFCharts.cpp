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


#include "QFCharts.h"
#include "QFChart.h"
#include "QFSeries.h"
#include "QFAxis.h"
#include "QFCrosshair.h"

#define QML_NAME "QFCharts"
#define QML_MAJOR 1
#define QML_MINOR 0


void registerQFChartsTypes() {
	qmlRegisterType<QFChart>(QML_NAME, QML_MAJOR, QML_MINOR, "QFChart");
	qRegisterMetaType<QFAbstractAxis*>();
	qmlRegisterType<QFTimeAxis>(QML_NAME, QML_MAJOR, QML_MINOR, "QFTimeAxis");
	qmlRegisterType<QFValueAxis>(QML_NAME, QML_MAJOR, QML_MINOR, "QFValueAxis");
	qmlRegisterType<QFCrosshair>(QML_NAME, QML_MAJOR, QML_MINOR, "QFCrosshair");
	qRegisterMetaType<QFAbstractSeries*>();
	qmlRegisterType<QFCandles>(QML_NAME, QML_MAJOR, QML_MINOR, "QFCandles");
	qmlRegisterType<QFVolumes>(QML_NAME, QML_MAJOR, QML_MINOR, "QFVolumes");
	qmlRegisterType<QFLines>(QML_NAME, QML_MAJOR, QML_MINOR, "QFLines");
}
