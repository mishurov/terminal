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


#include "TitleBarWindows.h"

#include <QtCore/QOperatingSystemVersion>

#define LOAD_SYMBOL(lib, name) LoadMethod(lib, #name, name)


void configureTitleBar(WId winId)
{
	static const auto kOsVersion = QOperatingSystemVersion::current();
	if (kOsVersion.microVersion() < 18875)
		return;

	if (!SetWindowCompositionAttribute) {
		const auto LibUser32 = LoadLibrary(L"user32.dll");
		LOAD_SYMBOL(LibUser32, SetWindowCompositionAttribute);
	}

	HWND hwnd = reinterpret_cast<HWND>(winId);
	BOOL dark = TRUE;
	WINDOWCOMPOSITIONATTRIBDATA data = {
		WINDOWCOMPOSITIONATTRIB::WCA_USEDARKMODECOLORS,
		&dark,
		sizeof(dark)
	};
	SetWindowCompositionAttribute(hwnd, &data);

	SendMessage(hwnd, WM_NCACTIVATE, 0, 0);
	SendMessage(hwnd, WM_NCACTIVATE, 1, 0);
}
