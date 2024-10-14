
#include "ini_handler_private.h"

#define TAG "INIHANDLER"
#define INI_RX_FILENAME "/spiffs/rx.ini"
#define INI_TX_FILENAME "/spiffs/tx.ini"

INIHNDLR_t INIHNDLR = {0};

void INIHANDLER_WriteToINI_RX(char *buffer, uint64_t size)
{
    FILE *ini;

    if ((ini = fopen(INI_RX_FILENAME, "w")) == NULL)
    {
        ESP_LOGE(TAG, "Cannot create %s\n", INI_RX_FILENAME);
        return;
    }

    int err = fprintf(ini,
                      "%.*s", (int)size, buffer);
    if (err < 0)
    {
        ESP_LOGE("Cannot write into %s", INI_RX_FILENAME);
    }

    fclose(ini);
}

void INIHANDLER_PrintINI(const char *filename)
{
    FILE *ini_file;

    if ((ini_file = fopen(INI_RX_FILENAME, "r")) == NULL)
    {
        ESP_LOGE(TAG, "Cannot create %s\n", INI_RX_FILENAME);
        return;
    }

    char ini_file_text[512];

    int i = 0;

    int c;
    while ((c = fgetc(ini_file)) != EOF)
    {
        // All alphabet and `\n`
        if ((c >= 32 && c <= 126) || c == 10)
        {
            snprintf(&ini_file_text[i], sizeof(int), "%c", (char)c);
            i++;
        }
        // Increase delay to prevent Watchdog Timeout
        vTaskDelay(50 / portTICK_PERIOD_MS); // 50 ms delay to yield more time
    }
    if (i > 0)
    {
        ESP_LOGI(TAG, "%.*s", i, ini_file_text);
    }

    fclose(ini_file);
}

void INIHANDLER_ParseCommand(char * buffer, uint32_t size)
{
    INIHNDLR.com_buf_size = size;
    INIHNDLR.command_buf = buffer;
    xTaskCreate(INIHANDLER_ParseTask, NULL, 2048, NULL, configMAX_PRIORITIES - 2, NULL);

}

static void INIHANDLER_ParseTask(void * a)
{
    ESP_LOGI(TAG, "PARSE!");
    dictionary *dict;
    
    dict = iniparser_load(INIHNDLR.command_buf, INIHNDLR.com_buf_size);

    if (dict == NULL)
    {
        ESP_LOGE(TAG, "cannot parse file: %s\n", INI_RX_FILENAME);
        return;
    }

    int i = iniparser_getint(dict, "player1:shape", -1);

    ESP_LOGI(TAG, "GOT %d", i);
}

