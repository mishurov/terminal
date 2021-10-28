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


Item {
    id: control

    property int align: Qt.AlignRight
    property real value: 0
    property real stepSize: 1
    property real from: 0
    property real to: 999999999

    height: 25
    width: 90

    onValueChanged: () => {
        const intValue = value * spinbox.factor;
        if (spinbox.value != intValue)
            spinbox.value = intValue;
    }

    SpinBoxExtended {
        id: spinbox
        property int decimals: 2
        readonly property int factor: Math.pow(10, decimals)

        onValueChanged: () => {
            const realValue = value / factor;
            if (control.value != realValue)
                control.value = realValue;
        }

        from: control.from * factor
        to: control.to * factor
        stepSize: control.stepSize * factor

        align: control.align

        width: control.width
        height: control.height

        validator: DoubleValidator {
            bottom: Math.min(spinbox.from, spinbox.to)
            top:  Math.max(spinbox.from, spinbox.to)
        }

        textFromValue: function(value, locale) {
            return Number(value / factor).toLocaleString(locale, 'f', spinbox.decimals)
        }

        valueFromText: function(text, locale) {
            return Number.fromLocaleString(locale, text) * factor
        }
    }
}
