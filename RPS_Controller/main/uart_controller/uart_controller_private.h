#pragma once
#include "uart_controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

typedef struct{
    volatile bool is_data_retrieved;
    char rx_buffer[2048];
    uint32_t rx_size;
    char tx_buffer[2048];
    uint32_t tx_size;
} UARTHNDLR_t;



static void UARTCNTRL_RX_Task(void * a);

