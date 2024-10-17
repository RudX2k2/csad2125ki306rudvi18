cmake_minimum_required(VERSION 3.16)

set(TEST_SOURCES
    testuarttxrx.cpp
)

# Find Qt Test module
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

# Define the test executable
add_executable(tests ${TEST_SOURCES})

# Link the test executable with the UartTxRx library and Qt Test module
target_link_libraries(tests PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    UartTxRx
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core  # Ensure QtCore is linked
    Qt${QT_VERSION_MAJOR}::SerialPort
)
