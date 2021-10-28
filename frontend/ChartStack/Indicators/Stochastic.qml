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
import './common.js' as C


QFChart {
    id: stochastic
    property alias timeAxis: timeAxis
    property alias valueAxis: valueAxis
    property alias mouseArea: mouseArea

    property var kQValues: []
    property var dQValues: []
    property var kValues: []
    property var dValues: []
    property int offset: 0

    property real infoPadding: 10

    function stoch(periodK, periodD, smooth, data) {
        const close = d => d ? d.close : null,
            high = d => d ? d.high : null,
            low = d => d ? d.low : null;
        const defined = d => close(d) != null && high(d) != null && low(d) != null;

        const acc = values => {
            const maxH = values && C.max(values, high);
            const minL = values && C.min(values, low);
            const kval = values && (
            100 * (close(values[values.length - 1]) - minL) / (maxH - minL));
            return isNaN(kval) ? undefined : kval;
        }

        const kwin = (data) => C.sliding(periodK, v => v, data, defined, acc)

        const kvals = C.sma(smooth, kwin(data), d => d);
        const dvals = C.sma(periodD, kvals, d => d);

        return kvals.map((k, i) => ({ k: k, d: dvals[i] }));
    }

    function clearSeriesPoints() {
        kValues = [];
        dValues = [];
    }

    function addSeriesPoints() {
        if (!api.candles.length) {
            return;
        } else {
            clearSeriesPoints();
            const sd = stoch(14, 3, 3, api.candles);
            offset = sd.findIndex(d => d.k != undefined && d.d != undefined) + 1
            if (!offset)
                return;
            addPoints(sd);
        }
        valueAxis.doZoom();
    }

    function updateLastPoint(isNew) {
        if (!offset)
            return;
        const sliced = api.candles.slice(api.candles.length - offset, api.candles.length);
        const sd = stoch(14, 3, 3, sliced);
        const latest = sd[sd.length - 1];
        const latestX = api.candles.length - 1
        const ptK = Qt.point(latestX, latest.k);
        const ptD = Qt.point(latestX, latest.d);
        if (!isNew) {
            kQValues.pop();
            dQValues.pop();
        }
        kQValues = kQValues.concat(ptK);
        dQValues = dQValues.concat(ptD);
        kValues[latestX] = latest.k;
        dValues[latestX] = latest.d;
    }

    function addPoints(sd) {
        const ptsK = [];
        const ptsD = [];
        for (var i = 0; i < sd.length; i++) {
            const pt = sd[i];
            kValues.push(pt.k);
            dValues.push(pt.d);
            if (pt.k == undefined || pt.d == undefined)
                continue
            ptsK.push(Qt.point(i, pt.k))
            ptsD.push(Qt.point(i, pt.d))
        }
        kQValues = ptsK;
        dQValues = ptsD;
    }

    function init() {
        addSeriesPoints();
        api.candlesAdded.connect(addSeriesPoints);
        api.candlesRemoved.connect(clearSeriesPoints);
        api.lastCandleUpdated.connect(updateLastPoint);
    }

    function deinit() {
        api.candlesAdded.disconnect(addSeriesPoints);
        api.candlesRemoved.disconnect(clearSeriesPoints);
        api.lastCandleUpdated.disconnect(updateLastPoint);
    }

    MouseArea {
        id: mouseArea
        onPositionChanged: mouse => {
            let idx = stochastic.mapToValue(Qt.point(mouse.x, 0)).x;
            idx = Math.round(idx);
            if (kValues[idx] != undefined)
                kText.text = kValues[idx].toFixed(2);
            if (dValues[idx] != undefined)
                dText.text = dValues[idx].toFixed(2);
        }
    }

    onHeightChanged: () => valueAxis.doZoom()

    QFTimeAxis {
        id: timeAxis
        parentAxis: {
            ApplicationWindow.window
            && ApplicationWindow.window.chartStack.main.timeAxis
        }
        onRangeChanged: (min, max) => valueAxis.doZoom()
    }

    QFValueAxis {
        id: valueAxis
        paddingPlotFraction: 0.15
        minimumTickInterval: 20

        function doZoom() {
            if (!kValues.length || !dValues.length)
                return;
            const minVisibleIdx = Math.max(0, Math.ceil(timeAxis.min)),
                  maxVisibleIdx = Math.min(kValues.length - 1, Math.floor(timeAxis.max));
            if (minVisibleIdx >= maxVisibleIdx)
                return;
            const visibleKValues = kValues.slice(minVisibleIdx, maxVisibleIdx + 1);
            const visibleDValues = dValues.slice(minVisibleIdx, maxVisibleIdx + 1);
            const visibleValues = visibleKValues.concat(visibleDValues, 20, 80).filter(x => x != undefined);
            const maxValue = Math.max.apply(null, visibleValues);
            const minValue = Math.min.apply(null, visibleValues);
            valueAxis.niceZoom(minValue, maxValue);
            shade.adjust();
        }
    }

    QFLines {
        axisX: timeAxis
        axisY: valueAxis
        lineColor: '#2873d6'
        lineWidth: 1
        dataset: kQValues
    }

    QFLines {
        id: stSeries2
        axisX: timeAxis
        axisY: valueAxis
        lineColor: '#ed2f4c'
        lineWidth: 1
        dataset: dQValues
    }

    Rectangle {
        id: shade

        function adjust() {
            const mapped80 = stochastic.mapToPosition(Qt.point(0, 80)).y;
            y = Math.max(mapped80, 0);
            const mapped20 = stochastic.mapToPosition(Qt.point(0, 20)).y;
            const mappedHeight = Math.abs(mapped80 - mapped20);
            const safeMaxY = Math.min(y + mappedHeight,
                stochastic.plotArea.y + stochastic.plotArea.height);
            height = safeMaxY - y;
        }

        z: -1
        x: stochastic.plotArea.x
        width: stochastic.plotArea.width
        color: '#34313d'
    }

    InfoBackground {}

    Row {
        id: info
        x: stochastic.plotArea.x + stochastic.infoPadding
        y: stochastic.plotArea.y + stochastic.infoPadding
        spacing: 5
        Text {
            text: 'Stochastic 14 3 3'
            font.pixelSize: 9
            color: "#d4d0c5"
        }
        Text {
            id: kText
            font.pixelSize: 9
            color: "#ed2f4c"
        }
        Text {
            id: dText
            font.pixelSize: 9
            color: "#2873d6"
        }
    }
}
