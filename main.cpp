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

#include "frontend/QFCharts/QFCharts.h"
#include "frontend/Blackfriars/Blackfriars.h"

#include <QtWidgets/QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>


int main(int argc, char *argv[])
{
	registerQFChartsTypes();
	initBlackfriars();

	QApplication app(argc, argv);
	QQmlApplicationEngine engine;

	//const QUrl url(QStringLiteral("qrc:/main.qml"));
	const QUrl url = QUrl::fromLocalFile("../frontend/main.qml");

	engine.load(url);
	if (engine.rootObjects().isEmpty())
		return -1;
	// antialiasing
	QQuickWindow* window = (QQuickWindow*) engine.rootObjects().first();
	QSurfaceFormat format = window->format();
	format.setSamples(4);
	window->setFormat(format); 
	return app.exec();
}
