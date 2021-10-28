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

ListModel {
    property var source: 'https://github.com/mishurov/terminal'
    property var actions: {
        'Quit': () => Qt.quit(),
        'Preferences': () => root.openPreferences(),
        'Source Code': () => Qt.openUrlExternally(source),
        'Stochastic': () => chartStack.toggleIndicator('Stochastic', true),
        'SMA': () => chartStack.toggleIndicator('SimpleMovingAverage', false)
    }

    ListElement {
        name: 'File'
        subMenu: [
            ListElement { name: 'Preferences' },
            ListElement { name: 'Quit' }
        ]
    }

    ListElement {
        name: 'Indicators'
        subMenu: [
            ListElement { name: 'Stochastic'; check: true },
            ListElement { name: 'SMA'; check: true }
        ]
    }

    ListElement {
        name: 'View'
        subMenu: [
            ListElement { name: 'Single View'; disabled: true },
            ListElement { name: 'Triple View'; disabled: true }
        ]
    }

    ListElement {
        name: 'Help'
        subMenu: [
            ListElement { name: 'Source Code' },
            ListElement { name: 'About'; disabled: true }
        ]
    }
}
