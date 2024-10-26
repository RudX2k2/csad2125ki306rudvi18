# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(RPS_Controller)

# Add this line if you are using a custom partition file
set(PARTITION_TABLE_CUSTOM ${CMAKE_SOURCE_DIR}/partitions.csv)
idf_build_set_property(PARTITION_TABLE_CUSTOM ${PARTITION_TABLE_CUSTOM})