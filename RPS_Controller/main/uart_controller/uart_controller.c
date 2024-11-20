#include "uart_controller_private.h"
#include "../ini_handler/ini_handler.h"

#define INIHANDLER_SENDDATA_STASKSIZE 4096
#define INIHANDLER_GETDATA_STASKSIZE 2048
static const int RX_BUF_SIZE = 1024;

static const char *TAG = "UARTHANDL";
#define CLEINT_MSG "Message from client"

UARTHNDLR_t UARTHNDL = {
    .is_data_retrieved = false,
    .is_new_data_collecting = false,
    .written_tx_len = 0,
};

// SemaphoreHandle_t parseDataUartSemaphore;

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

    // // Create a binary semaphore
    // parseDataUartSemaphore = xSemaphoreCreateBinary();
    // if (parseDataUartSemaphore == NULL)
    // {
    //     ESP_LOGE(TAG, "error to create semaphore");
    // }

    // Initialize semaphore to available
    // xSemaphoreGive(parseDataUartSemaphore);
}


static void UARTCNTRL_RX_Task(void *a)
{
    uint8_t data[1024];

    // Configure UART for longer timeout
    uart_set_rx_timeout(UART_NUM_0, 10);  // 10 symbol times timeout
    
    while (1)
    {
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, 1024, 500 / portTICK_PERIOD_MS);
        if (rxBytes > 0)
        {
            // ESP_LOGD(TAG, "Received %d bytes", rxBytes);
            
            if ((UARTHNDL.rx_size + rxBytes) < RX_BUF_SIZE)
            {
                memcpy(&(UARTHNDL.rx_buffer[UARTHNDL.rx_size]), data, rxBytes);
                UARTHNDL.rx_size += rxBytes;
                
                // Keep reading for a short while to ensure we get all data
                int empty_reads = 0;
                while (empty_reads < 5) {  // Try up to 5 times
                    vTaskDelay(pdMS_TO_TICKS(20));  // Wait a bit
                    int more_bytes = uart_read_bytes(UART_NUM_0, data, 1024, 100 / portTICK_PERIOD_MS);
                    if (more_bytes > 0) {
                        if ((UARTHNDL.rx_size + more_bytes) < RX_BUF_SIZE) {
                            memcpy(&(UARTHNDL.rx_buffer[UARTHNDL.rx_size]), data, more_bytes);
                            UARTHNDL.rx_size += more_bytes;
                            empty_reads = 0;  // Reset counter if we got data
                        }
                    } else {
                        empty_reads++;
                    }
                }
                
                // Now process the complete message
                // ESP_LOGW(TAG, "Complete data received, size: %d", UARTHNDL.rx_size);
                // ESP_LOGW(TAG, "Data: %s", UARTHNDL.rx_buffer);
                
                INIHANDLER_ParseCommand((char *)UARTHNDL.rx_buffer, UARTHNDL.rx_size);
                memset(UARTHNDL.rx_buffer, 0, RX_BUF_SIZE);
                UARTHNDL.rx_size = 0;
            }
            memset(data, 0, 1024);
        }
    }
}

void UARTCNTRL_SendData(char *buffer, uint32_t size)
{
    memcpy(UARTHNDL.tx_buffer, buffer, size);
    UARTHNDL.tx_size = size;

    ESP_LOGE(TAG, "ControllerData:%.*s", (int)size+1, buffer);

    uart_write_bytes(UART_NUM_0, UARTHNDL.tx_buffer, UARTHNDL.tx_size);
}

void UARTCNTRL_EnableRXDataPolling(void)
{
    // ESP_LOGI(TAG, "Enable rx polling");
    xTaskCreate(UARTCNTRL_RX_Task, "uart_rx_task", INIHANDLER_SENDDATA_STASKSIZE, NULL, configMAX_PRIORITIES - 1, NULL);
}
