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


#ifndef CHARTPANE_H
#define CHARTPANE_H

#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ChartPane; }
class QAbstractButton;
QT_END_NAMESPACE


class ChartPane : public QWidget
{
	Q_OBJECT

public:
	ChartPane(QWidget *parent=0);
	~ChartPane();

	Ui::ChartPane *ui;
	
	int interval();

signals:
	void intervalSet();

private:
	void intervalToggled(QAbstractButton *button, bool checked);

	int m_interval;
};


#endif // CHARTPANE_H
