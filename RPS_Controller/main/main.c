/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_log.h"
#include "ini_handler/ini_handler.h"
#include "uart_controller/uart_controller.h"
#include "emulator/emulator.h"

void app_main(void)
{
    UARTCNTRL_Init();
    UARTCNTRL_EnableRXDataPolling();

    EMULATOR_Init();

    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust as needed
    }
}
