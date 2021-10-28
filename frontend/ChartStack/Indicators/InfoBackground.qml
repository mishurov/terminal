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


Rectangle {
    id: infoBackground
    property real padding: 5
    x: info.x - padding
    y: info.y - padding
    width: info.width + padding * 2
    height: info.height + padding * 2
    radius: 7
    color: Qt.rgba(0.1, 0.1, 0.1, 0.3)
}
