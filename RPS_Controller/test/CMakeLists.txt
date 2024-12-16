idf_component_register(SRCS "main.c"
                            "ini_handler/ini_handler.c"
                            "ini_handler/ini_handler.c"
                            "ini_handler/iniparser/ini.c"
                            "ini_handler/iniparser/ini.c"
                            "uart_controller/uart_controller.c"
                            "emulator/emulator.c"
                            "test_emulator.c"
                    REQUIRES unity freertos driver)

set(EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS} $ENV{IDF_PATH}/tools/unit-test-app/components)

                    
# Create Unity library
add_library(unity STATIC
    $ENV{IDF_PATH}/tools/unit-test-app/components/unity/unity.c
)

# Link Unity with test executable
target_link_libraries(test_main unity)

# Optional: Add ESP-IDF specific flags or configurations
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNITY_INCLUDE_CONFIG_H")