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

import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.settings
import Blackfriars
import '../Controls'


Window {
    id: window
    property var p: Facade.preferences
    property int intialBroker: settings.value(p.brokerKey, p.brokerDefault)

    color: palette.window
    title: 'Preferences'

    width: 480
    height: 200

    Component.onCompleted: {
        Facade.configureWindowTitleBar(title)
        // issues on Linux
        palette.buttonText = '#eee'
    }

    Settings { id: settings }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: 2
        PrefLabel {
            Layout.alignment: Qt.AlignRight
            text: 'Broker'
        }
        ComboBoxRounded {
            id: broker
            Layout.fillWidth: true
            height: 20
            model: p.brokers
            currentIndex: intialBroker
        }

        PrefLabel {
            Layout.alignment: Qt.AlignRight
            text: 'Credentials'
        }
        TextField {
            id: credentials
            Layout.fillWidth: true
            text: settings.value(p.credentialsKey, '')
        }

        PrefLabel {
            Layout.alignment: Qt.AlignRight
            text: 'Default Risk, %'
        }
        DoubleSpinBox {
            id: risk
            from: 0
            to: 5
            stepSize: 0.25
            value: settings.value(p.riskKey, p.riskDefault)
        }

        PrefLabel {
            Layout.alignment: Qt.AlignRight
            text: 'Default Target, %'
        }
        DoubleSpinBox {
            id: target
            from: 0
            stepSize: 0.25
            value: settings.value(p.targetKey, p.targetDefault)
        }

        Item {
            id: flexibleSpace
            Layout.fillHeight: true
        }
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10
        spacing: 5
        ButtonColor {
            text: 'Cancel'
            onClicked: window.close()
        }
        ButtonColor {
            text: 'Ok'
            onClicked: () => {
                settings.setValue(p.brokerKey, broker.currentIndex);
                if (intialBroker != broker.currentIndex) {
                    api.brokerChanged(broker.currentIndex);
                    intialBroker = broker.currentIndex;
                }
                settings.setValue(p.credentialsKey, credentials.text);
                settings.setValue(p.riskKey, risk.value);
                settings.setValue(p.targetKey, target.value);

                window.close();
            }
        }
    }
}
