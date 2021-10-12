# /**************************************************************************
#
#  This file is part of the Blackfriars Trading Terminal
#
#  Copyright (C) 2021 Alexander Mishurov
#
#  GNU General Public License Usage
#  This file may be used under the terms of the GNU
#  General Public License version 3. The licenses are as published by
#  the Free Software Foundation and appearing in the file LICENSE.GPL3
#  included in the packaging of this file. Please review the following
#  information to ensure the GNU General Public License requirements will
#  be met: https://www.gnu.org/licenses/gpl-3.0.html.
#
# **************************************************************************/

cmake_minimum_required(VERSION 3.14)

project(Terminal LANGUAGES CXX)

set(QT_VERSION_STR 6.2.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# https://packages.debian.org/bullseye/amd64/libxcb-util1/download
# sudo apt install -t buster-backports libvulkan-dev

# Windows Git Bash
# export PATH=$PATH:/c/Qt/Tools/mingw810_64/bin/:/c/Qt/Tools/CMake_64/bin/
# mkdir build
# cd build
# cmake -G "MinGW Makefiles" ..
# mingw32-make.exe
if (APPLE)
    set(QT_COMPILER_DIRNAME macos)
elseif (UNIX)
    set(QT_COMPILER_DIRNAME gcc_64)
elseif (WIN32)
    set(QT_COMPILER_DIRNAME mingw81_64)
endif()

set(QT_PARENT_DIR $ENV{HOME})
set(CMAKE_PREFIX_PATH ${QT_PARENT_DIR}/Qt/${QT_VERSION_STR}/${QT_COMPILER_DIRNAME})

find_package(QT NAMES Qt6 COMPONENTS Widgets Charts Network WebSockets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts Network WebSockets REQUIRED)

include_directories( ${Qt6Charts_PRIVATE_INCLUDE_DIRS} )

set(PROJECT_SOURCES
    main.cpp
    resources.qrc
    frontend/palette/palette.cpp
    frontend/mainwindow/mainwindow.cpp
    frontend/mainwindow/mainwindow.ui
    frontend/mainwindow/centralwidget.cpp
    frontend/mainwindow/centralwidget.ui
    frontend/rightpane/rightpane.cpp
    frontend/rightpane/rightpane.ui
    frontend/preferences/prefdialog.cpp
    frontend/preferences/prefdialog.ui
    frontend/tickerinput/tickerinput.cpp
    frontend/chartpane/chartpane.cpp
    frontend/chartpane/finchartview.cpp
    frontend/chartpane/fintimeaxis.cpp
    frontend/chartpane/chartpane.ui
    frontend/chartpane/crosshair.cpp
    frontend/chartpane/shade.cpp
    frontend/alert/alert.cpp
    frontend/facade/market.cpp
    backend/controller/market.cpp
    backend/controller/preferences.cpp
    backend/brokers/abstractbroker.cpp
    backend/brokers/montecarlobroker.cpp
    backend/brokers/tinkoffbroker.cpp
)

if (APPLE)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} frontend/mainwindow/titlebar.mm)
endif()

qt_add_executable(Terminal MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(Terminal PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebSockets
)

qt_finalize_executable(Terminal)
