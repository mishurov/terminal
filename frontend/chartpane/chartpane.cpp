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


#include "chartpane.h"
#include "ui_chartpane.h"

#include <QtWidgets/QButtonGroup>
#include <QtCore/QSettings>

ChartPane::ChartPane(QWidget *parent)
	:QWidget(parent)
	, ui(new Ui::ChartPane)
{
	ui->setupUi(this);
	
	setStyleSheet(R"(
QPushButton {
	margin: 0;
	padding: 3 6 3 6;
	width: 25;
	font-size: 10px;
	color: #ddd;
}
QPushButton::checked {
	color: #777;
}
	)");

	auto btgr = ui->groupTimeIntervals;
	auto buttons = btgr->buttons();
	QAbstractButton *button = nullptr;

	QSettings settings("Mishurov", "Terminal");
	auto pref = settings.value("ChartPane/interval").toString();

	if (!pref.isEmpty()) {
		for (auto b : buttons) {
			if (b->text() == pref) {
				button = b;
			}
		}
	}

	if (button == nullptr)
		button = buttons.first();

	button->setChecked(true);
	m_interval = button->text().replace("m", "").toInt();

	connect(btgr,
		QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
		this, &ChartPane::intervalToggled);
}

ChartPane::~ChartPane()
{
	delete ui;
}

void ChartPane::intervalToggled(QAbstractButton *button, bool checked) {
	if (!checked)
		return;
	m_interval = button->text().replace("m", "").toInt();
	QSettings settings("Mishurov", "Terminal");
	settings.setValue("ChartPane/interval", button->text());

	emit intervalSet();
}

int ChartPane::interval() { return m_interval; }
