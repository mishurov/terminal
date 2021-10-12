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


#include "frontend/mainwindow/mainwindow.h"
#include "frontend/palette/palette.h"

#include "frontend/facade/market.h"
#include "backend/controller/market.h"

#include <QtWidgets/QApplication>
#include <QtCore/QTimer>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setStyle("Fusion");
	a.setPalette(AppPalette());

	MainWindow w;
	w.readSettings();
	w.show();
	// create controllers before exec to access widgets in facade constructors
	auto mc = MarketController(&a);
	QTimer::singleShot(0, mc.facade(), &MarketFacade::onEventLoopStarted);

	return a.exec();
}
