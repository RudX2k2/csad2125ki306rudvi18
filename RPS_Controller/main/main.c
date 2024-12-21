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
#include "esp_err.h"
#include "esp_log.h"
#include "../components/ini_handler/include/ini_handler.h"
#include "../components/uart_controller/include/uart_controller.h"
#include "../components/emulator/include/emulator.h"

void app_main(void)
{
    EMULATOR_Init();
    UARTCNTRL_Init();
    UARTCNTRL_EnableRXDataPolling();


    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust as needed
    }
}
