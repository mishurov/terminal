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


#include "prefdialog.h"
#include "ui_prefdialog.h"

#include "backend/controller/preferences.h"

#include <QtCore/QSettings>
#include <QtWidgets/QPushButton>

#include <QtCore/QDebug>


PrefDialog::PrefDialog(QWidget *parent)
	:QDialog(parent)
	, ui(new Ui::PrefDialog)
{
	ui->setupUi(this);

	auto cancelButton = ui->dialogButtons->button(QDialogButtonBox::Cancel);
	auto okButton = ui->dialogButtons->button(QDialogButtonBox::Ok);

	connect(cancelButton, &QPushButton::clicked, this, &PrefDialog::handleCancel);
	connect(okButton, &QPushButton::clicked, this, &PrefDialog::handleOk);

	readSettings();
}

PrefDialog::~PrefDialog()
{
	delete ui;
}

void PrefDialog::handleCancel()
{
	close();
}

void PrefDialog::handleOk()
{
	QSettings settings("Mishurov", "Terminal");

	PrefData data;
	data.broker = brokers.indexOf(ui->inputBroker->currentText());
	data.credentials = ui->inputCreds->text();
	data.risk = ui->inputDefaultRisk->value();
	data.target = ui->inputDefaultTarget->value();

	preferences.save(&data);

	close();
}

void PrefDialog::closeEvent(QCloseEvent *event)
{
	QSettings settings("Mishurov", "Terminal");
	settings.setValue("PrefDialog/geometry", saveGeometry());
	QDialog::closeEvent(event);
}

void PrefDialog::readSettings()
{
	QSettings settings("Mishurov", "Terminal");
	restoreGeometry(settings.value("PrefDialog/geometry").toByteArray());

	auto data = preferences.read();

	for (auto b : brokers) {
		ui->inputBroker->addItem(b);
	}
	ui->inputBroker->setCurrentText(brokers.at(data->broker));

	ui->inputCreds->setText(data->credentials);
	ui->inputDefaultRisk->setValue(data->risk);
	ui->inputDefaultTarget->setValue(data->target);
}
