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
import Blackfriars
import './constants.js' as C


QtObject {
    id: api
    /*
    First priority TODO s
    - unravel Tinkoff Broker, the code is too clutched and unreadable, separate logic from network and parsing
    - separate candles signals on appended, prepended, removed, updated last, added one new
    - use abstract panel indicator and abstract overlay indicator as bases
    - TSM AdjustCapsLockLEDForKeyTransitionHandling - _ISSetPhysicalKeyboardCapsLockLED Inhibit (MacOS)
    */
    signal candlesAdded()
    signal candlesRemoved()
    signal lastCandleUpdated(bool isNew)

    readonly property var stockModel: StockItemModel {}
    property var prevLastCandle: null
    property var prevFirstCandle: null
    property var candles: Facade.candles
    property var intervals: Facade.intervals
    property var figi: ''
    property int interval: 0
    property real prevBidAskAverage: 0
    property real bidAskAverage: Facade.bidAskAverage
    property real prevDayClose: Facade.prevDayClose

    function brokerChanged(index) {
        figi = '';
        topPane.clear();
        Facade.clearAllCandles();
        Facade.clearBrokerData();
        Facade.brokerChanged(index);
    }

    function requestCandles() {
        if (!interval || figi == '')
            return;
        Facade.clearAllCandles();
        chartStack.main.selectedInterval = interval;
        Facade.requestCandles(figi, new Date(), interval, 1000);
    }

    function onComboIndexChanged() {
        const reversed = intervals.slice().reverse();
        interval = reversed[chartStack.combo.currentIndex] || 0;
    }

    function requestPreCandles(numCandles) {
        const capacity = C.maxNumCandles - candles.length;
        if (capacity < 1)
            return;
        const numToRequest = Math.min(capacity, numCandles);
        const start = candles[0].time;
        let timestamp;
        if (interval < 99999) {
            timestamp = new Date(start.getTime() - interval * 60000);
        } else {
            timestamp = new Date(start);
            timestamp.setMonth(timestamp.getMonth() - 1);
        }
        Facade.requestCandles(figi, timestamp, interval, numToRequest);
    }

    onIntervalsChanged: () => {
        if (!intervals.length) {
            interval = 0;
            chartStack.combo.model = [];
            return;
        }
        const reversed = intervals.slice().reverse();
        chartStack.combo.model = reversed.map(x => C.intervalToString(x));
        chartStack.combo.currentIndex = reversed.length - 1;
        interval = reversed[reversed.length - 1];
    }

    onCandlesChanged: () => {
        const lastCandle = candles[candles.length - 1];
        const firstCandle = candles[0];
        if (!candles.length) {
            chartStack.main.clearCandles();
            api.candlesRemoved();
        } else {
            if (prevFirstCandle && firstCandle.time.getTime() === prevFirstCandle.time.getTime()) {
                const isNew = lastCandle.time.getTime() > prevLastCandle.time.getTime();
                chartStack.main.updateLastCandle(isNew);
                api.lastCandleUpdated(isNew);
            } else {
                chartStack.main.pushFrontCandles();
                api.candlesAdded();
            }
            chartStack.updatePriceLine(lastCandle);
        }
        prevFirstCandle = firstCandle;
        prevLastCandle = lastCandle;
    }

    onBidAskAverageChanged: () => {
        if (bidAskAverage && !prevBidAskAverage)
            rightPane.setStops(bidAskAverage);
        prevBidAskAverage = bidAskAverage;
    }

    onFigiChanged: () => requestCandles()
    onIntervalChanged: () => requestCandles()

    Component.onCompleted: () => {
        chartStack.combo.currentIndexChanged.connect(onComboIndexChanged);
        Facade.initWithQmlApi(api);
        Facade.requestBrokerData();
    }
}
