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
import Qt.labs.platform


MenuBar {
    id: menuBar
    property var menuModel: MenuModel {}

    Component.onCompleted: {
        // checkable doesn't work with instantiator
        for (let i = 0; i < menuModel.count; i++) {
            const menuData = menuModel.get(i);
            const menu = Qt.createQmlObject(
                'import Qt.labs.platform; Menu {}', menuBar, `menu_${i}`);
            menu.title = menuData.name;
            menuBar.addMenu(menu);
            for (let j = 0; j < menuData.subMenu.count; j++) {
                const itemData = menuData.subMenu.get(j);
                const item = Qt.createQmlObject(
                    'import Qt.labs.platform; MenuItem {}',
                    menu, `submenu_${i}_${j}`);
                item.text = itemData.name;
                item.enabled = !itemData.disabled;
                item.checkable = itemData.check;
                item.triggered.connect(() => menuModel.actions[itemData.name]());
                menu.addItem(item);
            }
        }
    }
}

