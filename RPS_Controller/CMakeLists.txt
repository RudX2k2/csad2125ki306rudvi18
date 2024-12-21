# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# include($ENV{IDF_PATH}/tools/unit-test-app)

# TODO for test purposes - remove when all will work ;(
# set(BUILD_TESTS_AMOGUS 1)
set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components" "main")
# set(EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS} $ENV{IDF_PATH}/tools/unit-test-app/components)


if(NOT BUILD_TESTS_AMOGUS EQUAL 1)
    set(EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/main)
endif()

if(BUILD_TESTS_AMOGUS EQUAL 1)
    set(EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/test)
endif()

project(RPS_Controller)
