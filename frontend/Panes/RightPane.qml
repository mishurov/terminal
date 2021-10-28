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
import Qt.labs.settings
import Blackfriars
import '../Controls'


Column {
    id: rightPane

    property real latestPrice: api.bidAskAverage;
    property real prevDayPrice: api.prevDayClose;
    property alias target: target
    property alias risk: risk

    function setStops(price) {
        const p = Facade.preferences;
        target.percent.value = settings.value(p.targetKey, p.targetDefault);
        target.dollar.value = price * (1 + target.percent.value / 100);
        chartStack.targetLine.setPosFromValue(target.dollar.value);

        risk.percent.value = settings.value(p.riskKey, p.riskDefault);
        risk.dollar.value = price * (1 - risk.percent.value / 100);
        chartStack.riskLine.setPosFromValue(risk.dollar.value);
    }

    function connectLines() {
        for (const key of ['target', 'risk']) {
            chartStack[key + 'Line'].yChanged.connect(() => {
                rightPane[key].dollar.value = chartStack[key + 'Line'].getValue().y;
            });
            rightPane[key].dollar.valueChanged.connect(() => {
                if (chartStack[key + 'Line'].mouseArea.drag.active)
                    return;
                chartStack[key + 'Line'].setPosFromValue(rightPane[key].dollar.value);
            });
        }
    }

    Component.onCompleted: () => {
        connectLines();
    }

    anchors.fill: parent
    anchors.margins: 10
    spacing: 5

    Settings { id: settings }

    ButtonGroup {
        buttons: longShort.children
    }

    Row {
        id: longShort
        width: parent.width

        ButtonColor {
            width: parent.width / 2
            text: 'Long'
            checked: true
            checkable: true
        }
        ButtonColor {
            width: parent.width / 2
            checkable: true
            text: 'Short'
            gradient1: '#ad063e'
            gradient2: '#bc0945'
        }
    }

    PaneLabel {
        text: 'Market Price ($)'
    }

    Row {
        spacing: 7
        Text {
            id: marketPrice
            anchors.verticalCenter: parent.verticalCenter
            property real value: rightPane.latestPrice
            text: value.toFixed(2)
            font.pixelSize: 25
            font.bold: true
            color: '#fff'
        }
        Text {
            id: marketChange
            anchors.baseline: marketPrice.baseline
            property real value: {
                if (rightPane.prevDayPrice)
                    return (rightPane.latestPrice - rightPane.prevDayPrice)
                        / rightPane.prevDayPrice;
                else
                    return 0;
            }
            text: (value > 0 ? '+' : '') + value.toFixed(2) + '%'
            font.pixelSize: 15
            color: value >= 0 ? '#02ed60' : '#fc1b35'
        }
    }

    PaneLabel {
        text: 'Stop'
    }
    StopRow {
        id: risk
    }

    PaneLabel {
        text: 'Target'
    }
    StopRow {
        id: target
    }

    PaneLabel {
        text: 'Return / Risk Ratio'
    }
    Text {
        text: '1.8 / 1'
        font.pixelSize: 15
        font.bold: true
        color: '#fff'
    }

    Button {
        text: 'Adjust Stops'
        width: parent.width
        height: 22
    }

    PaneLabel {
        text: 'Position Size'
    }
    Row {
        width: parent.width
        spacing: 5
        SpinBoxExtended {
            id: positionInput
            from: 0
            width: parent.width / 2
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            id: positionDollar
            text: '$'
            font.pixelSize: 10
            color: '#aaa'
        }
        Text {
            property real value: positionInput.value * marketPrice.value
            anchors.verticalCenter: parent.verticalCenter
            id: positionValue
            text: value.toFixed(2)
            font.pixelSize: 13
            font.bold: true
            color: '#fff'
        }
    }

    Button {
        text: 'Place Order'
        width: parent.width
        height: 50
    }
}
