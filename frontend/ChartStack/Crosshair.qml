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
import QtQuick.Shapes
import QFCharts
import '../constants.js' as C


Item {
    id: crosshair

    property bool isVertical: false
    property bool isDraggable: false
    property bool isOnlyMain: false
    property bool followAxis: false
    property color labelColor: '#eee'
    property color labelTextColor: '#333'
    property color lineColor: Qt.rgba(labelColor.r, labelColor.g, labelColor.b, 0.7)
    property var dashPattern: [3, 5]
    property real strokeWidth: 1
    property var chartView: main
    property var lineText: ''
    property color lineTextColor: labelColor
    property real valueY: 0
    property real valueX: 0
    property alias linePos: linePos

    property real offsetX: main.plotArea.x
    property real offsetY: main.plotArea.y

    property alias mouseArea: mouseArea

    function checkVisibility(posX, posY) {
        const maxHeight = isOnlyMain ? main.plotArea.height : chartStack.comboHeight;
        visible = (posX >= 0 && posX < main.plotArea.width &&
                   posY >= 0 && posY < maxHeight);
    }

    function setPositionFromGlobal(posX, posY) {
        if (isDraggable)
            return;
        const g = parent.mapFromGlobal(Qt.point(posX, posY));
        posX = g.x - offsetX;
        posY = g.y - offsetY;

        checkVisibility(posX, posY);

        if (isVertical) {
            const val = chartView.mapToValue(Qt.point(g.x, g.y));
            val.x = Math.round(val.x);
            const pos = chartView.mapToPosition(val);
            //crosshair.x = pos.x - offsetX;
            linePos.x = pos.x - offsetX;
        } else {
            //crosshair.y = posY;
            linePos.y = posY;
        }
    }

    function setPosFromValue(value) {
        if (isVertical && valueX != value)
            valueX = value;
        if (!isVertical && valueY != value)
            valueY = value;
        const pt = isVertical ? Qt.point(value, 0) : Qt.point(0, value);
        const mappedPos = chartView.mapToPosition(pt);
        const pos = isVertical ? mappedPos.x - offsetX : mappedPos.y - offsetY;
        if (isVertical) {
            checkVisibility(pos, 1);
            //crosshair.x = pos;
            linePos.x = pos;
        } else {
            checkVisibility(1, pos);
            //crosshair.y = pos;
            linePos.y = pos;
        }
    }

    function getValue() {
        let g = parent.mapToGlobal(Qt.point(linePos.x + offsetX, linePos.y + offsetY));
        g = chartView.mapFromGlobal(g.x, g.y);
        return chartView.mapToValue(Qt.point(g.x, g.y));
    }

    function getText() {
        const val = getValue();
        if (!crosshair.isVertical)
            return val.y.toFixed(2);
        const c = api.candles[Math.round(val.x)];
        if (!c)
            return 'n/a';
        const format = "d MMM yy h:mm";
        const fakeUTC = new Date(c.time.toUTCString().slice(0, -4));
        const ret = fakeUTC
            .toLocaleString(Qt.locale(), format)
            .replace(/([a-zA-Z] )/, "$1'");
        return ret;
    }

    width: main.plotArea.width
    height: chartStack.comboHeight

    onChartViewChanged: () => {
        if (!isOnlyMain)
            return;
        crosshair.visible = crosshair.visible && chartView == main;
    }

    Component.onCompleted: () => {
        if (!isOnlyMain || !followAxis)
            return;
        if (isVertical) {
            main.timeAxis.rangeChanged.connect(() => {
                setPosFromValue(valueX)
            });
        } else {
            main.priceAxis.rangeChanged.connect(() => {
                setPosFromValue(valueY)
            });
            // for some reason rangeChanged does not always
            // execute the connected handler
            main.heightChanged.connect(() => {
                setPosFromValue(valueY)
            });
        }
    }

    QFCrosshair {
        id: shape
        anchors.fill: parent
        orientation: isVertical ? Qt.Vertical : Qt.Horizontal
        strokeColor: crosshair.lineColor
        strokeWidth: crosshair.strokeWidth
        dashPattern: crosshair.dashPattern
        linePos: Qt.point(parent.linePos.x, parent.linePos.y)
    }
/*
    ShapePath {
        id: linePath
        strokeColor: crosshair.lineColor
        strokeStyle: ShapePath.DashLine
        dashPattern: crosshair.dashPattern
        strokeWidth: crosshair.strokeWidth
        fillColor: 'transparent'
        startX: shapePos.x
        startY: shapePos.y

        PathLine {
            relativeX: crosshair.isVertical ? 0 : crosshair.width
            relativeY: crosshair.isVertical ? crosshair.height : 0
        }
    }
*/
    Rectangle {
        x: isVertical ? offsetX - width / 2  + linePos.x : offsetX + parent.width
        y: isVertical ? offsetY + parent.height: offsetY - height / 2 + linePos.y
        width: isVertical ? 105 : 50
        height: isVertical ? 17 : 15
        radius: 3
        color: crosshair.labelColor

        Text {
            anchors.centerIn: parent
            text: crosshair.getText()
            font.pixelSize: 11
            color: crosshair.labelTextColor
        }
    }

    Text {
        x: { isVertical
            ? (linePos.x > 20 ? offsetX - 33 : offsetX - 14) + linePos.x
            : crosshair.width - 50 }
        y: { isVertical
             ? 60
             : (linePos.y > 20 ? offsetY - 12 : offsetY + 3) + linePos.y }
        rotation: isVertical ? -90 : 0
        visible: lineText != ''
        text: lineText
        font.pixelSize: 9
        color: lineTextColor
    }

    Item {
        id: linePos
        width: isVertical ? 1 : parent.width
        height: isVertical ? parent.height : 1
        MouseArea {
            id: mouseArea
            x: isVertical ? offsetX - C.crossHairSize / 2 : offsetX
            y: isVertical ? offsetY : offsetY - C.crossHairSize / 2
            width: isVertical ? C.crossHairSize : parent.width
            height: isVertical ? parent.height : C.crossHairSize
            visible: isDraggable
            enabled: isDraggable
            cursorShape: isVertical ? Qt.SplitHCursor : Qt.SplitVCursor
            drag.target: parent
            drag.smoothed: false
            drag.axis: isVertical ? Drag.XAxis : Drag.YAxis
            drag.maximumX: main.plotArea.width
            drag.maximumY: isOnlyMain ? main.plotArea.height : chartStack.comboHeight
            drag.minimumX: 0
            drag.minimumY: 0
            onPressed: () => {
                dragHandler.enabled = false;
            }
            onReleased: () => {
                dragHandler.enabled = true;
                const value = crosshair.getValue();
                crosshair.valueX = value.x;
                crosshair.valueY = value.y;
            }
        }
    }
}
