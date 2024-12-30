# Add to your main CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Enable code coverage
option(CODE_COVERAGE "Enable coverage reporting" OFF)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Add coverage flags
    add_compile_options(-O0 -g --coverage)
    add_link_options(--coverage)
endif()

# Enable CTest for test logging
enable_testing()

if(BUILD_TESTS)
    # Set CMake options for Qt
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    include_directories(${CMAKE_SOURCE_DIR})

    # Find Qt Test module
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets Core SerialPort)

    # Define test executable
    add_executable(unit_tests
        testuarttxrx.cpp
        testuarttxrx.h
        ../inibyteparser.cpp
        ../UartTxRx.cpp
    )

    # List of headers for AUTOMOC
    set(HEADERS
        ../gamedata.h
        ../inibyteparser.h
        ../UartTxRx.h
    )

    # Generate moc files for headers
    qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

    # Include the generated moc files in the test executable
    target_sources(unit_tests PRIVATE ${MOC_SOURCES})

    # Link the test executable with necessary libraries
    target_link_libraries(unit_tests PRIVATE
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::SerialPort
    )

    # Pass the TEST_PORT to the executable as a definition
    if(DEFINED TEST_PORT)
        target_compile_definitions(unit_tests PRIVATE TEST_PORT="${TEST_PORT}")
    endif()

    # Add a CTest test
    add_test(NAME UartTxRxTest COMMAND unit_tests)
endif()
