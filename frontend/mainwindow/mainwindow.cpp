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


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "frontend/preferences/prefdialog.h"

#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(ui->actionPref, &QAction::triggered, this, &MainWindow::openPref);

	connect(ui->actionSource, &QAction::triggered, this, &MainWindow::goToSource);
	ui->action3View->setEnabled(false);
	ui->actionAbout->setEnabled(false);

#if defined(Q_OS_MACOS)
	configureTitleBar(effectiveWinId());
#endif

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	auto focusWidget = QApplication::focusWidget();
	if (focusWidget)
		focusWidget->clearFocus();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QSettings settings("Mishurov", "Terminal");
	settings.setValue("MainWindow/geometry", saveGeometry());
	//settings.setValue("MainWindow/windowState", saveState());
	QMainWindow::closeEvent(event);
}

void MainWindow::readSettings()
{
	QSettings settings("Mishurov", "Terminal");
	restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
	//restoreState(settings.value("MainWindow/windowState").toByteArray());
}

void MainWindow::openPref()
{
	PrefDialog *pref = new PrefDialog;
	pref->show();
}

void MainWindow::goToSource()
{
	QString link = "https://github.com/mishurov/terminal";
	QDesktopServices::openUrl(QUrl(link));
}
