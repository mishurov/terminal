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
import Blackfriars
import QFCharts
import './common.js' as C


Item {
    id: sma

    property var main: null
    property var values: ({})
    property var points9: []
    property var points20: []
    property real infoPadding: 10
    property int offset: 0

    function onPositionChanged(mouse) {
        if (values[9] == undefined || values[20] == undefined)
            return;
        let idx = main.mapToValue(Qt.point(mouse.x, 0), main.mapSeries).x;
        idx = Math.round(idx);
        if (values[9][idx] != undefined)
            sma9text.text = values[9][idx].toFixed(2);
        if (values[20][idx] != undefined)
            sma20text.text = values[20][idx].toFixed(2);
    }

    function clearSeriesPoints() {
        values = {};
    }

    function addSeriesPoints() {
        if (!api.candles.length) {
            return;
        } else {
            clearSeriesPoints();
            createSMA(9);
            createSMA(20);
        }
    }

    function updateLastPoint(isNew) {
        if (!offset)
            return;

        const sliced = api.candles.slice(api.candles.length - offset, api.candles.length);

        for (const param of [9, 20]) {
            const points = param == 9 ? points9 : points20
            const ma = C.sma(param, sliced);
            const latest = ma[ma.length - 1];
            if (!isNew)
                points.pop();
            points.push(Qt.point(api.candles.length - 1, latest));
            values[param][api.candles.length - 1] = latest;
            if (param == 9)
                points9 = points;
            else
                points20 = points;
        }
    }

    function createSMA(param) {
        const ma = C.sma(param, api.candles);
        const dataset = [];
        const localOffset = ma.findIndex(d => d != undefined) + 1;
        if (!localOffset) {
            offset = 0;
            return;
        }
        offset = Math.max(offset, localOffset);

        values[param] = []
        const chartValues = [];
        const chartIndices = [];
        for (var i = 0; i < ma.length; i++) {
            values[param].push(ma[i])
            if (ma[i] !== undefined) {
                dataset.push(Qt.point(i, ma[i]));
            }
        }
        if (param == 9)
            points9 = dataset;
        else
            points20 = dataset;
    }

    function init() {
        main = parent
        main.addSeries(series9);
        main.addSeries(series20);
        createSMA(9);
        createSMA(20);
        api.candlesAdded.connect(addSeriesPoints);
        api.candlesRemoved.connect(clearSeriesPoints);
        api.lastCandleUpdated.connect(updateLastPoint);
        main.mouseArea.positionChanged.connect(onPositionChanged)
    }

    function deinit() {
        api.candlesAdded.disconnect(addSeriesPoints);
        api.candlesRemoved.disconnect(clearSeriesPoints);
        api.lastCandleUpdated.disconnect(updateLastPoint);
        main.removeSeries(series9);
        main.removeSeries(series20);
        main.mouseArea.positionChanged.disconnect(onPositionChanged)
    }

    QFLines {
        id: series9
        lineColor: '#f7705b'
        lineWidth: 1
        dataset: points9
    }

    QFLines {
        id: series20
        lineColor: '#1680dd'
        lineWidth: 1
        dataset: points20
    }

    InfoBackground {}

    Column {
        id: info
        x: ApplicationWindow.window.chartStack.main.plotArea.x + infoPadding
        y: ApplicationWindow.window.chartStack.main.plotArea.y + infoPadding
        spacing: 3
        Row {
            spacing: 5
            Text {
                text: 'SMA9'
                font.pixelSize: 9
                color: "#d4d0c5"
            }
            Text {
                id: sma9text
                font.pixelSize: 9
                color: "#f7705b"
            }
        }
        Row {
            spacing: 5
            Text {
                text: 'SMA20'
                font.pixelSize: 9
                color: "#d4d0c5"
            }
            Text {
                id: sma20text
                font.pixelSize: 9
                color: "#1680dd"
            }
        }
    }
}
