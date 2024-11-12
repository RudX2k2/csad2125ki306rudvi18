idf_component_register(SRCS "main.c"
                            "ini_handler/ini_handler.c"
                            "ini_handler/iniparser/ini.c"
                            "uart_controller/uart_controller.c"
                            "emulator/emulator.c"
                    INCLUDE_DIRS "ini_handler")

add_compile_options(-Wno-pointer-sign)

