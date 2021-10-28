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


#include "AppPalette.h"

AppPalette::AppPalette()
	:QPalette()
{
	auto bg = QColor(33, 32, 41);
	auto base = QColor(15, 15, 15);
	auto accent = QColor(20, 84, 247);
	auto disabled = QColor(105, 95, 100);

	this->setColor(QPalette::Window, bg);
	this->setColor(QPalette::WindowText, Qt::white);
	this->setColor(QPalette::Base, base);
	this->setColor(QPalette::AlternateBase, bg);
	this->setColor(QPalette::ToolTipBase, Qt::black);
	this->setColor(QPalette::ToolTipText, Qt::white);
	this->setColor(QPalette::Text, Qt::white);
	this->setColor(QPalette::Button, bg);
	this->setColor(QPalette::ButtonText, Qt::white);
	this->setColor(QPalette::BrightText, Qt::red);

	this->setColor(QPalette::Link, QColor(accent));
	this->setColor(QPalette::Highlight, QColor(accent));
	this->setColor(QPalette::HighlightedText, Qt::white);

	this->setColor(QPalette::Light, Qt::black);
	this->setColor(QPalette::Midlight, Qt::gray);
	this->setColor(QPalette::Mid, Qt::gray);
	this->setColor(QPalette::Dark, Qt::white);

	this->setColor(QPalette::Disabled, QPalette::Text, disabled);
	this->setColor(QPalette::Disabled, QPalette::Light, Qt::transparent);
}
