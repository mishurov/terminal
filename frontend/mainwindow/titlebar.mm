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


#import <AppKit/AppKit.h>

extern "C" void configureTitleBar(unsigned long winId)
{
	if (winId == 0) return;

	NSView* view = (NSView*)winId;
	NSWindow* window = [view window];
	window.titlebarAppearsTransparent = true;
	window.backgroundColor = [NSColor
		colorWithRed: (53 / 255.0f)
		green: (53 / 255.0f)
		blue: (53 / 255.0f)
		alpha: 1.0];
}
