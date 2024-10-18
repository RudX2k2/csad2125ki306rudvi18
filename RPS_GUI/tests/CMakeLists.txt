cmake_minimum_required(VERSION 3.16)
# Enable CTest for test logging
enable_testing()

if(BUILD_TESTS)
    set(CMAKE_AUTOMOC ON)  # Ensure AUTOMOC is enabled globally
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    include_directories(${CMAKE_SOURCE_DIR})

    # Find Qt Test module
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

    add_executable(unit_tests testuarttxrx.cpp
                                testuarttxrx.h)

    # Link the test executable with the UartTxRx library and Qt Test module
    target_link_libraries(unit_tests PRIVATE
        Qt${QT_VERSION_MAJOR}::Test
        UartTxRx
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::SerialPort
    )

# Add a CTest test
add_test(NAME UartTxRxTest COMMAND unit_tests)
endif()
