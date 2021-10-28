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
import Blackfriars
import QFCharts
import '../constants.js' as C


QFChart {
    id: main
    candles: api.candles

    property alias timeAxis: timeAxis
    property alias priceAxis: priceAxis
    property alias mouseArea: mouseArea

    readonly property int defaultCandleWidth: 7
    readonly property int defaultCandleOffset: 10
    readonly property real maxCandleWidth: 40
    readonly property real minCandleWidth: 0.3
    property var prevCandlesLength: 0
    property real currentCandleWidth: 7
    property var lastAngleDeltaY: 0
    property real lastPointerX: 0

    function clearCandles() {
        prevCandlesLength = 0;
    }

    function pushFrontCandles() {
        if (!prevCandlesLength) {
            doInitialZoom();
        } else {
            const diff = api.candles.length - prevCandlesLength;
            timeAxis.setRange(
                timeAxis.min + diff,
                timeAxis.max + diff,
            )
        }
        prevCandlesLength = api.candles.length;
    }

    function updateLastCandle(isNew) {
        const lastIdx = api.candles.length - 1;
        const candle = api.candles[lastIdx];
        const isIncreasing = candle.open < candle.close;
        if (isNew)
            timeAxis.updateForNewCandle();
    }

    function doInitialZoom() {
        const numCandles = Math.abs(main.plotArea.width) / defaultCandleWidth;
        const offset = numCandles - 2 > defaultCandleOffset ? defaultCandleOffset : 0;
        const maxTime = api.candles.length + offset;
        const minTime = maxTime - numCandles;
        timeAxis.setRange(minTime, maxTime);
        currentCandleWidth = defaultCandleWidth;
    }

    function wheelZoom(e) {
        lastAngleDeltaY += e.angleDelta.y

        const timeAxis = main.timeAxis
        const candlesRange = timeAxis.max - timeAxis.min;

        const base = 1.001,
            area = main.plotArea,
            factor = Math.pow(base, -lastAngleDeltaY),
            width = area.width * factor,
            x = area.right - width;

        lastAngleDeltaY = 0

        const lastIdx = api.candles.length - 1;
        //const candleWidth = width / (timeAxis.max - timeAxis.min);

        const lidx = main.mapToValue(Qt.point(x, 0)).x;
        const ridx = main.mapToValue(Qt.point(x + width, 0)).x;

        const candleWidth = width / (ridx - lidx);

        if ((timeAxis.min > lastIdx && factor < 1)
            || (candleWidth < minCandleWidth && factor > 1)
            || (candleWidth > maxCandleWidth && factor < 1))
            return;

        main.currentCandleWidth = candleWidth;
        const zoom = Qt.rect(x, area.y, width, area.height);
        main.zoomIn(zoom);
    }

    function dragPan(active, centroid) {
        if (!active) {
            lastPointerX = centroid.position.x;
            return;
        }

        const lastIdx = api.candles.length - 1;
        const timeAxis = main.timeAxis
        let deltaX = centroid.position.x - lastPointerX;

        lastPointerX = centroid.position.x;
        //if ((timeAxis.min < lastIdx && deltaX < 0)
            //|| (timeAxis.max > 0 && deltaX > 0))
            //main.scrollLeft(deltaX);
        if ((timeAxis.min > lastIdx && deltaX < 0)
            || (timeAxis.max < 0 && deltaX > 0))
            return;

        const lidx = main.mapToValue(Qt.point(plotArea.x - deltaX, 0)).x;
        const ridx = main.mapToValue(Qt.point(plotArea.x + plotArea.width - deltaX, 0)).x;

        main.scrollLeft(deltaX);
    }

    function gestureFinished() {
        if (!api.candles.length)
            return;
        const numCandles = Math.ceil(-timeAxis.min);
        if (numCandles > 0)
            api.requestPreCandles(numCandles);
    }

    onWidthChanged: () => {
        if (!api.candles.length)
            return;
        const candleNum = main.plotArea.width / currentCandleWidth;
        timeAxis.min = timeAxis.max - candleNum;
    }

    onHeightChanged: () => {
        if (splitView.resizing)
            return;
        chartStack.updateHeight();
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        Component.onCompleted: () => {
            positionChanged.connect(chartStack.getCrosshairPosHandler(parent));
            exited.connect(chartStack.getCrosshairExitHandler(parent));
        }
    }

    QFTimeAxis {
        id: timeAxis

        function updateForNewCandle() {
            if (api.candles.length - 1 >= timeAxis.max)
                return;
            timeAxis.setRange(timeAxis.min + 1, timeAxis.max + 1);
        }

        onRangeChanged: (min, max) => {
            const minIdx = Math.max(Math.round(min), 0);
            const maxIdx = Math.min(Math.round(max), api.candles.length - 1);
            const sliced = api.candles.slice(minIdx, maxIdx + 1);
            const pmin = Math.min.apply(null, sliced.map(c => c.low));
            const pmax = Math.max.apply(null, sliced.map(c => c.high));
            const vmax = Math.max.apply(null, sliced.map(c => c.volume));
            if (!isNaN(pmin) && !isNaN(pmax))
                priceAxis.niceZoom(pmin, pmax);
            if (!isNaN(vmax))
                volumeAxis.setRange(0, vmax * 2);
        }

        color: C.gridColor
        gridLineColor: C.gridLineColor
        labelsColor: C.gridLabelsColor
        labelsFont: Qt.font({pixelSize: 11})
    }

    QFValueAxis {
        id: priceAxis
        color: C.gridColor
        gridLineColor: C.gridLineColor
        labelsColor: C.gridLabelsColor
        //labelFormat: "%.2F"
        labelsFont: Qt.font({pixelSize: 11})
    }

    QFValueAxis {
        id: volumeAxis
        visible: false
    }

    QFVolumes {
        axisX: timeAxis
        axisY: volumeAxis
        barWidth: 0.9
        increasingColor: Qt.rgba(0.01, 0.58, 0.37, 0.5)
        decreasingColor: Qt.rgba(0.80, 0.02, 0.26, 0.5)
    }

    QFCandles {
        axisX: timeAxis
        axisY: priceAxis
        bodyWidth: 0.7
        increasingColor: '#01945e'
        decreasingColor: '#f21146'
    }
}
