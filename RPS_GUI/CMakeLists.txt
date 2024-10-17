cmake_minimum_required(VERSION 3.16)

project(RPS_GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort)  # Include QtCore

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Define the UartTxRx library
add_library(UartTxRx
    UartTxRx.cpp
    UartTxRx.h
)

# Link necessary Qt libraries to the UartTxRx library
target_link_libraries(UartTxRx PRIVATE
    Qt${QT_VERSION_MAJOR}::Core  # Ensure QtCore is linked
    Qt${QT_VERSION_MAJOR}::SerialPort
)

# Define the main executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RPS_GUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(RPS_GUI SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(RPS_GUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Link the main executable with UartTxRx and Qt Widgets
target_link_libraries(RPS_GUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets UartTxRx)

# Additional properties for macOS or Windows
set_target_properties(RPS_GUI PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RPS_GUI)
endif()

# Add the test directory
add_subdirectory(tests)
