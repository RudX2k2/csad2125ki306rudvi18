#include "uart_controller_private.h"
#include "../ini_handler/ini_handler.h"

static const int RX_BUF_SIZE = 1024;
#define INIHANDLER_SENDDATA_STASKSIZE 4096
#define INIHANDLER_GETDATA_STASKSIZE 2048

static const char *TAG = "UARTHANDL";
#define CLEINT_MSG "Message from client"

UARTHNDLR_t UARTHNDL = {
    .is_data_retrieved = false,
};

void UARTCNTRL_Init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t err;

    err = uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    if (err)
    {
        ESP_LOGE(TAG, "error %d", err);
    }

    err = uart_param_config(UART_NUM_0, &uart_config);
    if (err)
    {
        ESP_LOGE(TAG, "error %d", err);
    }
}


static void UARTCNTRL_RX_Task(void *a)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t data[RX_BUF_SIZE];

    while (1)
    {
        // Read bytes from UART with a 1000ms timeout
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0)
        {
            data[rxBytes] = 0; // Null-terminate the received data

            if (memcmp(data, CLEINT_MSG, sizeof(CLEINT_MSG)) == 0)
            {
                char *new_buf = NULL;
                int reply_msg_size = snprintf(NULL, 0, "%s%s", data, " modified message from the server");
                new_buf = (char *)malloc(reply_msg_size + 1);

                if (new_buf != NULL)
                {
                    snprintf(new_buf, reply_msg_size + 1, "%s%s", data, " modified message from the server");
                    UARTCNTRL_SendData(new_buf, reply_msg_size + 1);
                    free(new_buf);
                }
            }

            // INIHANDLER_ParseCommand((char *)data, rxBytes + 1);

            memset(data, 0, RX_BUF_SIZE);
        }
    }
}

void UARTCNTRL_SendData(char *buffer, uint32_t size)
{
    memcpy(UARTHNDL.tx_buffer, buffer, size);
    UARTHNDL.tx_size = size;

    uart_write_bytes(UART_NUM_0, UARTHNDL.tx_buffer, UARTHNDL.tx_size);
}

void UARTCNTRL_EnableRXDataPolling(void)
{
    ESP_LOGI(TAG, "Enable rx polling");
    xTaskCreate(UARTCNTRL_RX_Task, "uart_rx_task", INIHANDLER_SENDDATA_STASKSIZE, NULL, configMAX_PRIORITIES - 1, NULL);
}
