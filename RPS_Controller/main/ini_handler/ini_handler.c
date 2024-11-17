
#include "ini_handler_private.h"

#define TAG "INIHANDLER"
#define INI_RX_FILENAME "/spiffs/rx.ini"
#define INI_TX_FILENAME "/spiffs/tx.ini"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

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

void INIHANDLER_ParseCommand(char *buffer, uint32_t size)
{
    memset(INIHNDLR.command_buf, 0, INIHNDLR_CMDBUFF_SIZE);

    if (size < INIHNDLR_CMDBUFF_SIZE)
    {
        INIHNDLR.com_buf_size = size;
        memcpy(INIHNDLR.command_buf, buffer, size);
        xTaskCreate(INIHANDLER_ParseTask, NULL, 4096, NULL, configMAX_PRIORITIES - 2, NULL);
    }
    else
    {
        ESP_LOGE(TAG, "Can't ini parse - to large data");
    }
}

typedef struct
{
    const char *buffer;
    size_t buffer_size;
    size_t position;
} memory_buffer_t;

static char *memory_reader(char *str, int num, void *stream)
{
    memory_buffer_t *mbuf = (memory_buffer_t *)stream;

    if (mbuf->position >= mbuf->buffer_size)
    {
        return NULL; // EOF
    }

    size_t remaining = mbuf->buffer_size - mbuf->position;
    size_t to_read = (num < remaining) ? num : remaining;

    // Find end of line if present
    for (size_t i = 0; i < to_read; i++)
    {
        if (mbuf->buffer[mbuf->position + i] == '\n')
        {
            to_read = i + 1; // Include the newline
            break;
        }
    }

    // Copy data to output buffer
    memcpy(str, mbuf->buffer + mbuf->position, to_read);
    str[to_read] = '\0';
    mbuf->position += to_read;

    return (to_read > 0) ? str : NULL;
}

static void INIHANDLER_ParseTask(void *a)
{
    ClientMessage_t client_message;

    ESP_LOGW(TAG, "Data length:%d", INIHNDLR.com_buf_size);
    ESP_LOGW(TAG, "%s", INIHNDLR.command_buf);

    memory_buffer_t mbuf = {
        .buffer = INIHNDLR.command_buf,
        .buffer_size = INIHNDLR.com_buf_size,
        .position = 0};

    client_message.get_gamestate.isIncluded = false;
    client_message.set_player_turn.isIncluded = false;
    client_message.set_gameconfig.isIncluded = false;

    if (ini_parse_stream(memory_reader, &mbuf, INIHANDLER_ClientMessageParser, &client_message) < 0)
    {
        ESP_LOGE(TAG, "Can't parse ini file");
        vTaskDelete(NULL);
    }

    if (client_message.get_gamestate.isIncluded)
    {
        GameState_CommonData_t send_gamestate = EMULATOR_GetGameState();
        ESP_LOGI(TAG, "Get gamestate request");
        INIHANDLER_GetGameState(send_gamestate);
    }
    if (client_message.set_player_turn.isIncluded)
    {
        ESP_LOGI(TAG, "player_turn included");

        EMULATOR_SetNewRecievedTurn(client_message.set_player_turn.turn_result.mode);

        // Emualtor can proccess the turn
        EMULATOR_GiveSemIsTurnRetrieved();
    }
    if (client_message.set_gameconfig.isIncluded)
    {
        EMULATOR_SetGameInfo(client_message.set_gameconfig.isLoaded,
                             client_message.set_gameconfig.gamestate.mode,
                             client_message.set_gameconfig.gamestate.Player1,
                             client_message.set_gameconfig.gamestate.Player2,
                             client_message.set_gameconfig.gamestate.maxRounds);

        EMULATOR_GiveSemIsStateRetrieved();
    }
    vTaskDelete(NULL);
}

static int INIHANDLER_ClientMessageParser(void *user, const char *section, const char *name,
                                          const char *value)
{
    ClientMessage_t *client_msg = (ClientMessage_t *)user;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (strcmp(section, "SetGameConfig") == 0)
    {
        if (MATCH("SetGameConfig", "Client"))
        {
            int clientVal = strtol(value, NULL, 0);
            if (clientVal == 1)
            {
                client_msg->set_gameconfig.isIncluded = true;
            }
            else
            {
                client_msg->set_gameconfig.isIncluded = false;
            }
        }

        if (MATCH("SetGameConfig", "IsLoaded"))
        {
            client_msg->set_gameconfig.isLoaded = strtol(value, NULL, 0);
        }
        if (MATCH("SetGameConfig", "Mode"))
        {
            memcpy(client_msg->set_gameconfig.gamestate.mode, value, 4);
        }
        if (MATCH("SetGameConfig", "Player1"))
        {
            client_msg->set_gameconfig.gamestate.Player1 = strtol(value, NULL, 0);
        }
        if (MATCH("SetGameConfig", "Player2"))
        {
            client_msg->set_gameconfig.gamestate.Player2 = strtol(value, NULL, 0);
        }
        if (MATCH("SetGameConfig", "MaxRounds"))
        {
            client_msg->set_gameconfig.gamestate.maxRounds = strtol(value, NULL, 0);
        }
    }
    if (strcmp(section, "GetGameState") == 0)
    {
        if (MATCH("GetGameState", "Client"))
        {
            int clientVal = strtol(value, NULL, 0);
            if (clientVal == 1)
            {
                client_msg->get_gamestate.isIncluded = true;
            }
            else
            {
                client_msg->get_gamestate.isIncluded = false;
            }
        }
    }
    if (strcmp(section, "SetPlayerTurn") == 0)
    {
        if (MATCH("SetPlayerTurn", "Client"))
        {
            int clientVal = strtol(value, NULL, 0);
            if (clientVal == 1)
            {
                client_msg->set_player_turn.isIncluded = true;
                memcpy(client_msg->set_player_turn.turn_result.mode, value, 4);
            }
            else
            {
                client_msg->set_player_turn.isIncluded = false;
            }
        }
    }
    return 1;
}

void INIHANDLER_SendClientGoodConfig(void)
{
    char *message = "[GetConfigResult]\n"
                    "Server=1\n"
                    "Result=1\n";
    UARTCNTRL_SendData(message, strlen(message) + 1);
}

void INIHANDLER_SendWaitTurn(void)
{
    char *message = "[WaitClientTurn]\n"
                    "Server=1"
                    "WaitTurn=1\n";
    UARTCNTRL_SendData(message, strlen(message) + 1);
}

void INIHANDLER_GetTurnResult(GetTurnResult_CommonData_t turn_result)
{
    int get_turnResult_size = snprintf(NULL, 0, "[GetTurnResult]\n"
                                                "Server=1\n"
                                                "Mode=%s\n"
                                                "Player1=%d\n"
                                                "Player2=%d\n"
                                                "MaxRounds=%d\n"
                                                "Winner=%d\n",
                                       turn_result.mode,
                                       turn_result.player1,
                                       turn_result.player2,
                                       turn_result.max_rounds,
                                       turn_result.winner);

    char *get_turnResult = (char *)malloc(get_turnResult_size);

    snprintf(get_turnResult, get_turnResult_size + 1, "[GetTurnResult]\n"
                                                      "Server=1\n"
                                                      "Mode=%s\n"
                                                      "Player1=%d\n"
                                                      "Player2=%d\n"
                                                      "MaxRounds=%d\n"
                                                      "Winner=%d\n",
             turn_result.mode,
             turn_result.player1,
             turn_result.player2,
             turn_result.max_rounds,
             turn_result.winner);

    UARTCNTRL_SendData(get_turnResult, get_turnResult_size + 1);
    free(get_turnResult);
}

void INIHANDLER_GetGameState(GameState_CommonData_t gamestate)
{
    int get_gamestate_size = snprintf(NULL, 0, "[GetGameState]\n"
                                               "Server=1\n"
                                               "Mode=%s\n"
                                               "Player1=%d\n"
                                               "Player2=%d\n"
                                               "MaxRounds=%d\n"
                                               "Winner=%d\n",
                                      gamestate.mode,
                                      gamestate.Player1,
                                      gamestate.Player2,
                                      gamestate.maxRounds,
                                      gamestate.winner);

    char *get_gamestate = (char *)malloc(get_gamestate_size);

    snprintf(get_gamestate, get_gamestate_size + 1, "[GetGameState]\n"
                                                    "Server=1\n"
                                                    "Mode=%s\n"
                                                    "Player1=%d\n"
                                                    "Player2=%d\n"
                                                    "MaxRounds=%d\n"
                                                    "Winner=%d\n",
             gamestate.mode,
             gamestate.Player1,
             gamestate.Player2,
             gamestate.maxRounds,
             gamestate.winner);

    UARTCNTRL_SendData(get_gamestate, get_gamestate_size + 1);
    free(get_gamestate);
}
