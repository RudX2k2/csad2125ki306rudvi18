idf_component_register(SRCS "test_emulator.c"
    INCLUDE_DIRS "."
    REQUIRES unity freertos driver test_utils
)