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


#ifndef ALERT_H
#define ALERT_H

#include <QtWidgets/QMessageBox>


class Alert : QMessageBox {
public:
	Alert(QWidget *parent=nullptr);
	void up(QString text);
	void up(QString text, QString details);
};

#endif // ALERT_H
