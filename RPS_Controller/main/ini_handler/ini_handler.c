
#include "ini_handler_private.h"

#define TAG "INIHANDLER"
#define INI_RX_FILENAME "/spiffs/rx.ini"
#define INI_TX_FILENAME "/spiffs/tx.ini"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

INIHNDLR_t INIHNDLR = {0};

void INIHANDLER_ParseCommand(char *buffer, uint32_t size)
{
    memset(INIHNDLR.command_buf, 0, INIHNDLR_CMDBUFF_SIZE);
    if (size < INIHNDLR_CMDBUFF_SIZE)
    {
        INIHNDLR.com_buf_size = size;
        if (buffer == NULL)
        {
            ESP_LOGE(TAG, "Bad buffer");
        }
        else
        {
            memcpy(INIHNDLR.command_buf, buffer, size);
            xTaskCreate(INIHANDLER_ParseTask, NULL, 8192, NULL, configMAX_PRIORITIES - 2, NULL);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Can't ini parse - to large data or incorrect size");
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
    ClientMessage_t client_message = {0};

    // ESP_LOGW(TAG, "\n%s", INIHNDLR.command_buf);

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
        // ESP_LOGI(TAG, "Get gamestate request");
        INIHANDLER_GetGameState(send_gamestate);
    }
    if (client_message.set_player_turn.isIncluded)
    {
        // ESP_LOGI(TAG, "player_turn included");

        EMULATOR_SetNewRecievedTurn(client_message.set_player_turn.turn_result.turn);

        // Emualtor can proccess the turn
        EMULATOR_GiveSemIsTurnRetrieved();
    }
    if (client_message.set_gameconfig.isIncluded)
    {
        int err = EMULATOR_SetGameInfo(client_message.set_gameconfig.isLoaded,
                                       client_message.set_gameconfig.gamestate.mode,
                                       client_message.set_gameconfig.gamestate.Player1,
                                       client_message.set_gameconfig.gamestate.Player2,
                                       client_message.set_gameconfig.gamestate.currentRound,
                                       client_message.set_gameconfig.gamestate.maxRounds);
        if (err)
        {
            // Needed delay so PC could successfully read the message
            vTaskDelay(500 / portTICK_PERIOD_MS);
            INIHANDLER_SendClientConfigResult(0);
        }
        else
        {
            INIHANDLER_SendClientConfigResult(1);
            EMULATOR_GiveSemIsStateRetrieved();
        }
    }
    if (client_message.clean_game.isIncluded)
    {
        ESP_LOGI(TAG, "Clean game");
        EMULATOR_CleanGame();
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
        if (MATCH("SetGameConfig", "CurrentRound"))
        {
            client_msg->set_gameconfig.gamestate.currentRound = strtol(value, NULL, 0);
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
            }
            else
            {
                client_msg->set_player_turn.isIncluded = false;
            }
        }
        if (MATCH("SetPlayerTurn", "Turn"))
        {
            memcpy(client_msg->set_player_turn.turn_result.turn, value, 10);
        }
    }
    if (strcmp(section, "CleanGame") == 0)
    {
        if (MATCH("CleanGame", "Client"))
        {
            int clientVal = strtol(value, NULL, 0);
            if (clientVal == 1)
            {
                client_msg->clean_game.isIncluded = true;
            }
            else
            {
                client_msg->clean_game.isIncluded = false;
            }
        }
    }
    return 1;
}

void INIHANDLER_SendClientConfigResult(uint8_t result)
{
    // Use a sufficiently large buffer to accommodate the formatted string
    char send_configResult[128]; // Adjust size as needed for your format

    int send_configResultSize = snprintf(send_configResult, sizeof(send_configResult),
                                         "[GetConfigResult]\n"
                                         "Server=1\n"
                                         "Result=%d\n",
                                         result);

    if (send_configResultSize < 0 || send_configResultSize >= sizeof(send_configResult))
    {
        // Handle the error: message was truncated or formatting failed
        ESP_LOGE(TAG, "Error formatting config result. Buffer too small or snprintf failed.");
        return;
    }

    UARTCNTRL_SendData(send_configResult, send_configResultSize);
}

void INIHANDLER_SendWaitTurn(emulator_players_enum_t player)
{
    int player_num = player + 1;
    char send_waitTurn[128]; // Adjust size based on expected output

    int send_waitTurnSize = snprintf(send_waitTurn, sizeof(send_waitTurn),
                                     "[WaitClientTurn]\n"
                                     "Server=1\n"
                                     "WaitTurn=1\n"
                                     "Player=%d",
                                     player_num);

    if (send_waitTurnSize < 0 || send_waitTurnSize >= sizeof(send_waitTurn))
    {
        ESP_LOGE(TAG, "Error formatting wait turn message. Buffer too small or snprintf failed.");
        return;
    }

    UARTCNTRL_SendData(send_waitTurn, send_waitTurnSize);
}

void INIHANDLER_GetTurnResult(GetTurnResult_CommonData_t turn_result)
{
    char get_turnResult[256]; // Adjust size based on expected output

    int get_turnResult_size = snprintf(get_turnResult, sizeof(get_turnResult),
                                       "[GetTurnResult]\n"
                                       "Server=1\n"
                                       "Mode=%s\n"
                                       "Player1=%d\n"
                                       "Player2=%d\n"
                                       "CurrentRound=%d\n"
                                       "MaxRounds=%d\n"
                                       "ChoiceP1=%s\n"
                                       "ChoiceP2=%s\n"
                                       "Winner=%d\n",
                                       turn_result.mode,
                                       turn_result.player1,
                                       turn_result.player2,
                                       turn_result.cur_round,
                                       turn_result.max_rounds,
                                       turn_result.choice_p1,
                                       turn_result.choice_p2,
                                       turn_result.winner);

    if (get_turnResult_size < 0 || get_turnResult_size >= sizeof(get_turnResult))
    {
        ESP_LOGE(TAG, "Error formatting turn result message. Buffer too small or snprintf failed.");
        return;
    }

    UARTCNTRL_SendData(get_turnResult, get_turnResult_size);
}

void INIHANDLER_GetGameState(GameState_CommonData_t gamestate)
{
    char get_gamestate[128]; // Adjust size based on expected output

    int get_gamestate_size = snprintf(get_gamestate, sizeof(get_gamestate),
                                      "[GetGameState]\n"
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

    if (get_gamestate_size < 0 || get_gamestate_size >= sizeof(get_gamestate))
    {
        ESP_LOGE(TAG, "Error formatting game state message. Buffer too small or snprintf failed.");
        return;
    }

    UARTCNTRL_SendData(get_gamestate, get_gamestate_size);
}

void INIHANDLER_SendClientCleanOk(void)
{
    char clean_result[128]; // Adjust size based on expected output

    int clean_result_size = snprintf(clean_result, sizeof(clean_result),
                                      "[CleanResult]\n"
                                      "Server=1\n"
                                      "Result=1\n");

    if (clean_result_size < 0 || clean_result_size >= sizeof(clean_result))
    {
        ESP_LOGE(TAG, "Error formatting game state message. Buffer too small or snprintf failed.");
        return;
    }

    UARTCNTRL_SendData(clean_result, clean_result_size);
}