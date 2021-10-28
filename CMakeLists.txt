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

cmake_minimum_required(VERSION 3.19)

set(QT_VERSION_STR 6.2.1)

if (APPLE)
    set(QT_COMPILER_DIRNAME macos)
elseif (UNIX)
    # sudo apt install -t bullseye-backports libvulkan-dev
    set(QT_COMPILER_DIRNAME gcc_64)
elseif (WIN32)
    # Windows (Git BASH / MinGW)
    # add to PATH (Search env: Edit the system environment variables)
    # C:\Qt\6.2.1\mingw81_64\bin
    # C:\Qt\Tools\mingw810_64\bin
    # C:\Qt\Tools\CMake_64\bin
    # mkdir build
    # cd build
    # cmake -G "MinGW Makefiles" ..
    # mingw32-make.exe
    set(QT_COMPILER_DIRNAME mingw81_64)
endif()

if (WIN32)
    set(QT_PARENT_DIR "C:")
else()
    set(QT_PARENT_DIR $ENV{HOME})
endif()

set(CMAKE_PREFIX_PATH ${QT_PARENT_DIR}/Qt/${QT_VERSION_STR}/${QT_COMPILER_DIRNAME})

project(Blackfriars VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Quick QuickControls2 Widgets Network WebSockets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick QuickControls2 Widgets Network WebSockets REQUIRED)

set(PROJECT_SOURCES
    frontend/QFCharts/QFCharts.cpp
    frontend/QFCharts/QFCandle.h
    frontend/QFCharts/QFAxis.cpp
    frontend/QFCharts/QFCrosshair.cpp
    frontend/QFCharts/QFChart.cpp
    frontend/QFCharts/QFGeoNodes.cpp
    frontend/QFCharts/QFSeries.cpp
    #frontend/frontend.qrc
    frontend/Blackfriars/Blackfriars.cpp
    frontend/Blackfriars/Alert.cpp
    frontend/Blackfriars/AppPalette.cpp
    frontend/Blackfriars/Facade.cpp
    frontend/Blackfriars/StockItemModel.cpp
    backend/Controller/Controller.cpp
    backend/Brokers/brokers.qrc
    backend/Brokers/AbstractBroker.cpp
    backend/Brokers/MonteCarloBroker.cpp
    backend/Brokers/TinkoffBroker.cpp
    main.cpp
)

if (APPLE)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} frontend/Blackfriars/TitleBarMacOS.mm)
    set(MACOSX_BUNDLE_ICON_FILE blackfriars.icns)
    set(app_icon_macos
        "${CMAKE_CURRENT_SOURCE_DIR}/appicon/blackfriars.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${app_icon_macos})
elseif (WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} frontend/Blackfriars/TitleBarWindows.cpp)
    set(PROJECT_SOURCES ${PROJECT_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/appicon/blackfriars.rc")
endif()

if (APPLE)
    qt_add_executable(Blackfriars MACOSX_BUNDLE ${PROJECT_SOURCES})
else()
    qt_add_executable(Blackfriars ${PROJECT_SOURCES})
endif()

if (APPLE)
    set_target_properties(Blackfriars PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER finance.mishurov.co.uk
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

target_compile_definitions(Blackfriars
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(Blackfriars PRIVATE
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebSockets
)
