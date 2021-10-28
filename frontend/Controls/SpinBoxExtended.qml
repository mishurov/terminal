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


SpinBox {
    id: control
    property int align: Qt.AlignRight
    editable: true
    leftPadding: 5
    rightPadding: 22
    contentItem: TextInput {
        z: 2
        text: control.textFromValue(control.value, control.locale)
        font: control.font
        color: 'white'
        selectionColor: palette.highlight
        selectByMouse: true
        horizontalAlignment: control.align
        verticalAlignment: Qt.AlignVCenter
        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
    }
}
