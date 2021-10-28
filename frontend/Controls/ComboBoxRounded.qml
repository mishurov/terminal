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

import QtQuick
import QtQuick.Controls


ComboBox {
    id: control
    popup.background: Rectangle {
        color: palette.base
        radius: 5
    }
    delegate: ItemDelegate {
        id: item
        text: modelData
        font.bold: control.currentIndex == index
        highlighted: control.highlightedIndex == index
        background: Rectangle {
            color: item.highlighted ? palette.highlight : 'transparent'
            radius: 4
        }
        height: 25
        width: control.width - 2
    }
}
