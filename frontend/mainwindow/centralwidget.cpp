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


#include "centralwidget.h"
#include "ui_centralwidget.h"


CentralWidget::CentralWidget(QWidget *parent)
	:QWidget(parent)
	, ui(new Ui::CentralWidget)
{
	ui->setupUi(this);

	ui->splitter->setStyleSheet(R"(
		::handle{
			margin: 7;
		}
	)");

#if defined(Q_OS_MACOS)
	layout()->setContentsMargins(10, 30, 10, 10);
#endif
}

CentralWidget::~CentralWidget()
{
	delete ui;
}
