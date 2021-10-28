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

Button {
    id: control
    property color gradient1: '#066b4b'
    property color gradient2: '#057753'

    Rectangle {
        visible: control.checked
        border.color: Qt.rgba(1, 1, 1, 0.05)
        border.width: 1
        gradient: Gradient {
            GradientStop { position: 0.0; color: gradient1 }
            GradientStop { position: 1.0; color: gradient2 }
        }
        anchors.fill: control
        radius: 3
    }
}
