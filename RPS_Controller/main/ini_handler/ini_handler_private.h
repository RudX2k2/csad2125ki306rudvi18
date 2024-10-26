#pragma once

#include <stdio.h>

#include "ini_handler.h"

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "iniparser/dictionary.h"
#include "iniparser/iniparser.h"

#define INIFILE_SIZE 512

typedef struct{
    char * command_buf;
    size_t com_buf_size;
}INIHNDLR_t;

static void INIHANDLER_ParseTask(void * a);

