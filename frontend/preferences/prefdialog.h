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


#ifndef PREFDIALOG_H
#define PREFDIALOG_H

#include <QtWidgets/QDialog>
#include <QtCore/QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class PrefDialog; }
QT_END_NAMESPACE


class PrefDialog : public QDialog
{
	Q_OBJECT

public:
	PrefDialog(QWidget *parent=0);
	~PrefDialog();

	Ui::PrefDialog *ui;

	void handleCancel();
	void handleOk();

private:
	void readSettings();
	void closeEvent(QCloseEvent *event);
};

#endif
