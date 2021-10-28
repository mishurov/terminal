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
import QFCharts
import Blackfriars
import './Indicators'
import '../Controls'
import '../constants.js' as C


Item {
    id: chartStack

    property alias main: main
    property alias combo: combo
    property alias targetLine: targetLine
    property alias riskLine: riskLine
    property var indicators: ({})
    property var comp: null
    property double bottomMargin: 0
    property double comboHeight: 0

    function updatePriceLine(candle) {
        const isIncreasing = candle.open < candle.close;
        priceLine.labelColor = isIncreasing ? '#0dd865' : '#fc2828';
        priceLine.setPosFromValue(candle.close);
    }

    function toggleIndicator(name, isPanel) {
        if (indicators[name])
            destroyIndicator(name, isPanel);
        else
            createIndicator(name, isPanel);
    }

    function destroyIndicator(name, isPanel) {
        if (isPanel) {
            const prevItem = splitView.contentChildren[
                splitView.contentChildren.length - 2
            ];
            prevItem.timeAxis.labelsVisible = true
            splitView.removeItem(indicators[name]);
        }
        indicators[name].deinit();
        indicators[name].destroy();
        indicators[name] = null;
    }

    function createIndicator(name, isPanel) {
        comp = Qt.createComponent(`Indicators/${name}.qml`);
        let indicator;
        if (isPanel) {
            indicator = comp.createObject(splitView, {
                'SplitView.fillWidth': true,
                'SplitView.minimumHeight': 100,
                'implicitHeight': 120
            });
        } else {
            indicator = comp.createObject(main, {});
        }
        indicators[name] = indicator;
        if (isPanel)
            initPanelIndicator(name, indicator);
        else
            initOverlayIndicator(name, indicator);
    }

    function initOverlayIndicator(name, indicator) {
        indicator.init();
    }

    function initPanelIndicator(name, indicator) {
        for (const p of C.axisCopyProps) {
            indicator.timeAxis[p] = main.timeAxis[p];
            indicator.valueAxis[p] = main.priceAxis[p];
        }

        indicator.timeAxis.labelsFont = main.timeAxis.labelsFont;
        indicator.valueAxis.labelsFont = Qt.font({pixelSize: 10});
        indicator.mouseArea.anchors.fill = indicator;
        indicator.mouseArea.hoverEnabled = true;

        indicator.mouseArea.positionChanged.connect(
            getCrosshairPosHandler(indicator)
        );
        indicator.mouseArea.exited.connect(
            getCrosshairExitHandler(indicator)
        );

        const prevItem = splitView.contentChildren[
            splitView.contentChildren.length - 2
        ];

        prevItem.timeAxis.labelsVisible = false;
        indicator.width = main.width;
        indicator.init();
    }

    function updateHeight() {
        let h = 0;
        const length = splitView.contentChildren.length;
        for (let i = 0; i < length; i++)
            h += splitView.contentChildren[i].plotArea.height;
        h += (length - 1) * C.handleHeight
        comboHeight = h;
    }

    function getCrosshairPosHandler(chart) {
        return mouse => {
            const g = chart.mouseArea.mapToGlobal(mouse.x, mouse.y)
            crosshairX.setPositionFromGlobal(g.x, g.y);
            crosshairY.setPositionFromGlobal(g.x, g.y);
            crosshairX.chartView = chart;
            crosshairY.chartView = chart;
        };
    }

    function getCrosshairExitHandler(chart) {
        return mouse => {
            const g = chart.mouseArea.mapToGlobal(-1, -1)
            crosshairX.setPositionFromGlobal(g.x, g.y);
            crosshairY.setPositionFromGlobal(g.x, g.y);
        };
    }

    anchors.fill: parent

    Item {
        anchors.fill: parent
        anchors.topMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 30

        WheelHandler {
            id: wheelHandler
            orientation: Qt.Vertical
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: e => main.wheelZoom(e)
            onActiveChanged: () => !active && main.gestureFinished()
        }

        DragHandler {
            id: dragHandler
            target: null
            dragThreshold: 1
            yAxis.enabled: false
            onCentroidChanged: () => main.dragPan(active, centroid)
            onActiveChanged: () => !active && main.gestureFinished()
        }

        SplitView {
            id: splitView
            anchors.fill: parent
            orientation: Qt.Vertical

            handle: Item {
                readonly property var handle: SplitHandle
                implicitHeight: C.handleHeight

                Rectangle {
                    x: main.plotArea.x - 1
                    implicitWidth: main.plotArea.width + 2
                    implicitHeight: parent.height
                    Behavior on color {
                        ColorAnimation { duration: C.animationDuration }
                    }
                    color: handle.pressed ? C.handlePressed
                        : (handle.hovered ? C.handleOver : C.handleOut)
                }
            }

            MainChart {
                id: main
                SplitView.fillHeight: true
                SplitView.minimumHeight: 200
                /*
                SimpleMovingAverage {
                    Component.onCompleted: { init(); }
                }
                */
            }
            /*
            Stochastic {
                Component.onCompleted: { init(); }
            }
            */
        }

        Crosshair {
            id: targetLine
            isVertical: false
            isDraggable: true
            isOnlyMain: true
            followAxis: true
            dashPattern: [2, 3]
            labelColor: '#08dd93'
            lineText: 'take profit'
        }
        Crosshair {
            id: riskLine
            isVertical: false
            isDraggable: true
            isOnlyMain: true
            followAxis: true
            dashPattern: [2, 3]
            labelColor: '#f93b5e'
            lineText: 'stop loss'
        }
        Crosshair {
            id: priceLine
            isVertical: false
            isOnlyMain: true
            followAxis: true
            dashPattern: [1, 2]
        }
        Crosshair { id: crosshairX; isVertical: true }
        Crosshair { id: crosshairY; isVertical: false }
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.margins: 10
        anchors.right: parent.right
        ComboBoxRounded {
            id: combo
            width: 90
            height: 20
        }
    }
}
