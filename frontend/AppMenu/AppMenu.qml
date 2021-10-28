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
import QtQuick.Shapes


MenuBar {
    id: menuBar
    property var menuModel: MenuModel {}

    delegate: MenuBarItem {
        id: barItem
        background: Rectangle {
            color: barItem.hovered ? palette.highlight : 'transparent'
            radius: 3
            border.width: 0
        }
    }

    Instantiator {
        model: menuModel
        onObjectAdded: (i, m) => menuBar.insertMenu(i, m)
        onObjectRemoved: m => menuBar.removeMenu(m)
        delegate: Menu {
            id: menu
            title: name
            background: Rectangle {
                implicitWidth: 90
                color: palette.base
                radius: 5
                border.width: 0
            }
            delegate: MenuItem {
                id: menuItem
                background: Rectangle {
                    radius: 4
                    border.width: 0
                    border.color: palette.base
                    color: menuItem.hovered && menuItem.highlighted ? palette.highlight : 'transparent'
                }
                indicator: Rectangle {
                    visible: menuItem.checkable
                    anchors.left: parent.left
                    anchors.leftMargin: 6
                    anchors.verticalCenter: parent.verticalCenter
                    implicitWidth: 10
                    implicitHeight: 10
                    border.color: '#999'
                    border.width: 1
                    color: 'transparent'
                    radius: 3
                    Shape {
                        visible: menuItem.checked
                        anchors.fill: parent
                        ShapePath {
                            strokeColor: 'white'
                            strokeWidth: 1.5
                            fillColor: 'transparent'
                            startX: 2
                            startY: 4
                            PathLine { x: 5; y: 7 }
                            PathLine { x: 10; y: 0 }
                        }
                    }
                }
            }

            Instantiator {
                model: subMenu
                onObjectAdded: (i, a) => menu.insertAction(i, a)
                onObjectRemoved: a => menu.removeAction(a)
                delegate: Action {
                    text: name
                    enabled: !disabled
                    checkable: check
                    onTriggered: () => menuModel.actions[name]()
                }
            }
        }
    }
}
