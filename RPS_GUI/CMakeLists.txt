cmake_minimum_required(VERSION 3.16)
project(RPS_GUI VERSION 0.1 LANGUAGES CXX)

include(CTest)
option(BUILD_TESTS "Build tests" OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    Core
    SerialPort
)

# Remove duplicates from source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    gamewindow.cpp
    gamewindow.h
    gamewindow.ui
    gamedata.cpp
    gamedata.h
    inibyteparser.h
    inibyteparser.cpp
    appka.ui
)

# Define the UartTxRx library
add_library(UartTxRx
    UartTxRx.cpp
    UartTxRx.h
)

# Link necessary Qt libraries to the UartTxRx library
target_link_libraries(UartTxRx PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::SerialPort
)

if(NOT BUILD_TESTS)
    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(RPS_GUI
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(RPS_GUI ${PROJECT_SOURCES}
            CommonDataTypes.h)
    endif()

    target_link_libraries(RPS_GUI PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        UartTxRx
    )
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
