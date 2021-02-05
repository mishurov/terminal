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


#include "palette.h"

AppPalette::AppPalette()
	:QPalette()
{
	this->setColor(QPalette::Window, QColor(53, 53, 53));
	this->setColor(QPalette::WindowText, Qt::white);
	this->setColor(QPalette::Base, QColor(25, 25, 25));
	this->setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	this->setColor(QPalette::ToolTipBase, Qt::black);
	this->setColor(QPalette::ToolTipText, Qt::white);
	this->setColor(QPalette::Text, Qt::white);
	this->setColor(QPalette::Button, QColor(53, 53, 53));
	this->setColor(QPalette::ButtonText, Qt::white);
	this->setColor(QPalette::BrightText, Qt::red);

	this->setColor(QPalette::Link, QColor(20, 84, 247));
	this->setColor(QPalette::Highlight, QColor(20, 84, 247));
	this->setColor(QPalette::HighlightedText, Qt::white);

	this->setColor(QPalette::Disabled, QPalette::Text, QColor(65, 65, 65));
	this->setColor(QPalette::Disabled, QPalette::Light, Qt::transparent);
}
