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


#ifndef TITLEBAR_WINDOWS_H
#define TITLEBAR_WINDOWS_H
//https://github.com/telegramdesktop/tdesktop/blob/master/Telegram/SourceFiles/platform/win/windows_dlls.h
//https://github.com/telegramdesktop/tdesktop/blob/master/Telegram/SourceFiles/platform/win/main_window_win.cpp
//https://github.com/desktop-app/lib_base/blob/master/base/platform/win/base_windows_safe_library.h

#include <QtGui/QWindow>

#include <Windows.h>


enum class WINDOWCOMPOSITIONATTRIB {
	WCA_UNDEFINED = 0,
	WCA_NCRENDERING_ENABLED = 1,
	WCA_NCRENDERING_POLICY = 2,
	WCA_TRANSITIONS_FORCEDISABLED = 3,
	WCA_ALLOW_NCPAINT = 4,
	WCA_CAPTION_BUTTON_BOUNDS = 5,
	WCA_NONCLIENT_RTL_LAYOUT = 6,
	WCA_FORCE_ICONIC_REPRESENTATION = 7,
	WCA_EXTENDED_FRAME_BOUNDS = 8,
	WCA_HAS_ICONIC_BITMAP = 9,
	WCA_THEME_ATTRIBUTES = 10,
	WCA_NCRENDERING_EXILED = 11,
	WCA_NCADORNMENTINFO = 12,
	WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
	WCA_VIDEO_OVERLAY_ACTIVE = 14,
	WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
	WCA_DISALLOW_PEEK = 16,
	WCA_CLOAK = 17,
	WCA_CLOAKED = 18,
	WCA_ACCENT_POLICY = 19,
	WCA_FREEZE_REPRESENTATION = 20,
	WCA_EVER_UNCLOAKED = 21,
	WCA_VISUAL_OWNER = 22,
	WCA_HOLOGRAPHIC = 23,
	WCA_EXCLUDED_FROM_DDA = 24,
	WCA_PASSIVEUPDATEMODE = 25,
	WCA_USEDARKMODECOLORS = 26,
	WCA_LAST = 27
};

struct WINDOWCOMPOSITIONATTRIBDATA {
	WINDOWCOMPOSITIONATTRIB Attrib;
	PVOID pvData;
	SIZE_T cbData;
};

inline BOOL(__stdcall *SetWindowCompositionAttribute)(
	HWND hWnd,
	WINDOWCOMPOSITIONATTRIBDATA*);

template <typename Function>
bool LoadMethod(HINSTANCE library, LPCSTR name, Function &func, WORD id = 0) {
	if (!library) return false;

	auto result = GetProcAddress(library, name);
	if (!result && id) {
		result = GetProcAddress(library, MAKEINTRESOURCEA(id));
	}
	func = reinterpret_cast<Function>(result);
	return (func != nullptr);
}

void configureTitleBar(WId winId);


#endif // TITLEBAR_WINDOWS_H
