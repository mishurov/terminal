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


#include "Blackfriars.h"

#include "StockItemModel.h"
#include "Facade.h"
#include "AppPalette.h"
#include "backend/Controller/Controller.h"

#include <QtWidgets/QApplication>
#include <QtQuickControls2/QQuickStyle>

#define APP_STYLE "Fusion"

#define ORG_NAME "Mishurov Finance"
#define ORG_DOMAIN "finance.mishurov.co.uk"

#define QML_NAME "Blackfriars"
#define QML_MAJOR 1
#define QML_MINOR 0


void initBlackfriars() {
	QQuickStyle::setStyle(APP_STYLE);
	QApplication::setStyle(APP_STYLE);
	QApplication::setPalette(AppPalette());
	QApplication::setOrganizationName(ORG_NAME);
	QApplication::setOrganizationDomain(ORG_DOMAIN);
	QApplication::setApplicationName(QML_NAME);

	qmlRegisterType<StockItemModel>(QML_NAME, QML_MAJOR, QML_MINOR, "StockItemModel");
	// memory leak
	auto controller = new Controller();
	qmlRegisterSingletonInstance(QML_NAME, QML_MAJOR, QML_MINOR, "Facade", controller->facade());
}
