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
import QtQuick.Layouts
import Blackfriars
import 'ChartStack'
import 'Panes'
import 'Controls'


ApplicationWindow {
    id: root
    property color paneColor: '#2a282f'
    property alias chartStack: chartStack

    function openPreferences() {
        const component = Qt.createComponent('Preferences/Preferences.qml');
        const window = component.createObject(root);
        window.show();
    }

    width: 800
    height: 500
    visible: true
    color: palette.window
    title: 'Blackfriars'

    menuBar: Loader {
        id: menuBar
        width: parent.width
        source: { Qt.platform.os === 'osx'
            ? 'AppMenu/AppMenuMacOS.qml' : 'AppMenu/AppMenu.qml' }
    }

    API { id: api }

    Component.onCompleted: {
        Facade.configureWindowTitleBar(title)
        // issues on Linux
        palette.buttonText = '#eee'
    }

    Shortcut {
        sequence: StandardKey.Quit
        context: Qt.ApplicationShortcut
        onActivated: Qt.quit()
    }

    Popup {
        id: popup
        x: 100
        y: 100
        width: 200
        height: 300
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        Rectangle {
            color: paneColor
            radius: 5
            Layout.topMargin: Qt.platform.os === 'osx' ? 10 : 5
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            TopPane { id: topPane }
        }
        SplitView {
            spacing: 5
            Layout.bottomMargin: 10
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.fillHeight: true
            Layout.fillWidth: true

            handle: Item {
                property var split: SplitHandle
                implicitWidth: 16
                Rectangle {
                    implicitWidth: 4
                    implicitHeight: 50
                    anchors.centerIn: parent
                    radius: 5
                    Behavior on color {
                        ColorAnimation { duration: 300 }
                    }
                    color: split.pressed ? '#555'
                        : (split.hovered ? '#444' : '#34313d')
                }
            }
            Rectangle {
                SplitView.fillWidth: true
                color: paneColor
                radius: 10
                ChartStack { id: chartStack }
            }
            Rectangle {
                SplitView.minimumWidth: 220
                color: paneColor
                radius: 10
                RightPane { id: rightPane }
            }
        }
    }
}
