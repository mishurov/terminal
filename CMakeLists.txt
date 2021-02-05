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

cmake_minimum_required(VERSION 3.5)

project(Terminal LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# https://packages.debian.org/bullseye/amd64/libxcb-util1/download
# sudo apt install -t buster-backports libvulkan-dev
set(OpenGL_GL_PREFERENCE GLVND)

# Windows Git Bash
# export PATH=$PATH:/c/Qt/Tools/mingw810_64/bin/:/c/Qt/Tools/CMake_64/bin/
# mkdir build
# cd build
# cmake -G "MinGW Makefiles" ..
# mingw32-make.exe
if (APPLE)
    set(COMPILER clang)
elseif (UNIX)
    set(COMPILER gcc)
elseif (WIN32)
    set(COMPILER mingw81)
endif()

if (WIN32)
    set(QT_LOC "C:")
else()
    set(QT_LOC $ENV{HOME})
endif()

set(QT_VERSION_STR 5.15.2)

set(CMAKE_PREFIX_PATH ${QT_LOC}/Qt/${QT_VERSION_STR}/${COMPILER}_64)

find_package(QT NAMES Qt5 COMPONENTS Widgets Charts Network WebSockets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts Network WebSockets REQUIRED)

include_directories( ${Qt5Charts_PRIVATE_INCLUDE_DIRS} )

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
    find_library(AppKit AppKit)
    find_library(Foundation Foundation)
    set(APPLE_LIBS ${AppKit} ${Foundation})
    set(PROJECT_SOURCES ${PROJECT_SOURCES} frontend/mainwindow/titlebar.mm)
endif()

add_executable(Terminal
    ${PROJECT_SOURCES}
)

target_link_libraries(Terminal PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebSockets
    ${APPLE_LIBS}
)
