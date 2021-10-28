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
import QtQuick.Layouts
import QtQuick.Controls


RowLayout {
    anchors.fill: parent

    function clear() {
        textField.text = '';
        textField.focus = false;
        title.text = '';
    }

    Text {
        Layout.alignment: Qt.AlignVCenter|Qt.AlignLeft
        Layout.leftMargin: 7
        text: 'ticker'
        font.pixelSize: 11
        color: '#aaa'
    }

    TextField {
        id: textField
        Layout.alignment: Qt.AlignVCenter|Qt.AlignLeft
        onTextChanged: () => {
            const item = listView.currentItem;
            if (!text.length || (item && item.modelEntry.ticker.ticker == text)) {
                popup.close();
            } else {
                listView.model.setFilterFixedString(text);
                listView.model.sortResults();
                popup.open();
            }
        }
        Keys.forwardTo: [listView.currentItem, listView]
        font.capitalization: Font.AllUppercase
        font.bold: true

        function accept() {
            if (listView.currentIndex < 0)
                return;

            title.text = listView.currentItem.modelEntry.name;
            textField.text = listView.currentItem.modelEntry.ticker;
            textField.focus = false;
            api.figi = listView.currentItem.modelEntry.figi;
            popup.close();
        }

        Image {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 3
            mipmap: true
            smooth: true
            width: 15
            height: 15
            source: './magnifier.svg'

            MouseArea {
                anchors.fill: parent
                onClicked: () => textField.accept()
            }
        }
        Popup {
            id: popup
            padding: 0
            y: parent.height
            clip: true
            background: Rectangle {
                color: palette.base
                radius: 3
            }
            ListView {
                id: listView
                implicitHeight: contentHeight < 200 ? contentHeight : 200
                implicitWidth: 300
                currentIndex: 0
                model: api.stockModel
                header: null
                footer: null
                delegate: ItemDelegate {
                    property var modelEntry: {
                        'ticker': ticker,
                        'name': name,
                        'figi': figi,
                    }
                    background: Rectangle {
                        color: parent.highlighted ? palette.highlight : palette.base
                        radius: 3
                    }
                    height: 25
                    width: 300
                    Row {
                        x: 10
                        width: 290
                        anchors.centerIn: parent
                        Text {
                            text: model.ticker
                            color: '#eee'
                            width: 60
                            font.bold: true
                        }
                        Text {
                            text: model.name
                            color: '#bbb'
                        }
                    }
                    MouseArea {
                        anchors.fill: parent;
                        hoverEnabled: true
                        onEntered: () => listView.currentIndex = index
                        onClicked: () => textField.accept()
                    }
                    highlighted: index === listView.currentIndex
                    Keys.onReturnPressed: () => textField.accept()
                }
            }
        }
    }

    Text {
        id: title
        Layout.alignment: Qt.AlignVCenter|Qt.AlignLeft
        Layout.fillWidth: true
        Layout.leftMargin: 10
        font.pixelSize: 16
        color: '#fff'
    }
}
