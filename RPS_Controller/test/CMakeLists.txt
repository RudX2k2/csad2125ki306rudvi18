# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# Set the components to include the tests for.
# This can be overridden from CMake cache:
# - when invoking CMake directly: cmake -D TEST_COMPONENTS="xxxxx" ..
# - when using idf.py: idf.py -T xxxxx build
#

set(EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS} /home/rud/esp/esp-idf/tools/unit-test-app/components)


set(TEST_COMPONENTS "emulator" CACHE STRING "List of components to test")

# set(EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS} $ENV{IDF_PATH}/tools/unit-test-app/components)
# Create Unity library
add_library(unity STATIC
/home/rud/esp/esp-idf/tools/unit-test-app/components/test_utils/test_utils.c
)

idf_component_register(SRCS "main/some_unit_test.c"
                            "../components/ini_handler/ini_handler.c"
                            "../components/ini_handler/ini_handler.c"
                            "../components/ini_handler/include/iniparser/ini.c"
                            "../components/ini_handler/include/iniparser/ini.c"
                            "../components/uart_controller/uart_controller.c"
                            "../components/emulator/emulator.c"
                            "../components/emulator/test/test_emulator.c"
                            # "test_emulator.c"
                    REQUIRES unity freertos driver test_utils)



# Link Unity with test executable
# target_link_libraries(test_main unity)

# Optional: Add ESP-IDF specific flags or configurations
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNITY_INCLUDE_CONFIG_H")
add_compile_options(-Wno-error=format)
