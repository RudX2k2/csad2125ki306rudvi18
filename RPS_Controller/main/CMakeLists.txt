idf_component_register(SRCS "main.c"
                            "../components/ini_handler/ini_handler.c"
                            "../components/ini_handler/include/iniparser/ini.c"
                            "../components/uart_controller/uart_controller.c"
                            "../components/emulator/emulator.c"
                    REQUIRES freertos driver)