#pragma once
#include "uart_controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

typedef struct
{
    volatile bool is_data_retrieved;
    char rx_buffer[2048];
    uint32_t rx_size;
    char tx_buffer[2048];
    uint32_t tx_size;
    uint32_t written_tx_len;
    uint32_t new_data_counter;
    volatile bool is_new_data_collecting;
} UARTHNDLR_t;

#define UARTCNTRL_NEW_DATA_TIMEOUT 1000

static void UARTCNTRL_RX_Task(void *a);

static void UARTCNTRL_NewDataCounterTask(void *a);

