
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

    client_message.game_state.isIncluded = false;
    client_message.player_turn.isIncluded = false;

    if (ini_parse_stream(memory_reader, &mbuf, INIHANDLER_ClientMessageParser, &client_message) < 0)
    {
        ESP_LOGE(TAG, "Can't parse ini file");
        vTaskDelete(NULL);
    }

    if (client_message.game_state.isIncluded)
    {
        ESP_LOGI(TAG, "game_state included");
        EMULATOR_SetGameInfo(client_message.game_state.isGameToLoad,
                             client_message.game_state.mode,
                             client_message.game_state.player1_score,
                             client_message.game_state.player2_score,
                             client_message.game_state.maxRoundsAmount);

        EMULATOR_GiveSemIsStateRetrieved();
    }
    if (client_message.player_turn.isIncluded)
    {
        ESP_LOGI(TAG, "player_turn included");

        EMULATOR_SetNewRecievedTurn(client_message.player_turn.choice);

        // Emualtor can proccess the turn
        EMULATOR_GiveSemIsTurnRetrieved();
    }

    vTaskDelete(NULL);
}

void INIHANDLER_SendWaitTurn(void)
{
    char *message = "[ClientTurn]\n"
                    "Server=1"
                    "WaitTurn=1\n";
    UARTCNTRL_SendData(message, strlen(message) + 1);
}

static int INIHANDLER_ClientMessageParser(void *user, const char *section, const char *name,
                                          const char *value)
{
    ClientMessage_t *client_msg = (ClientMessage_t *)user;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (strcmp(section, "GameState") == 0)
    {
        if (MATCH("GameState", "Client"))
        {
            int clientVal = strtol(value, NULL, 0);
            if (clientVal == 1)
            {
                client_msg->game_state.isIncluded = true;
            }
            else
            {
                client_msg->game_state.isIncluded = false;
            }
        }

        if (MATCH("GameState", "IsLoaded"))
        {
            client_msg->game_state.isGameToLoad = strtol(value, NULL, 0);
        }
        if (MATCH("GameState", "Mode"))
        {
            memcpy(client_msg->game_state.mode, value, 4);
        }
        if (MATCH("GameState", "Player1"))
        {
            client_msg->game_state.player1_score = strtol(value, NULL, 0);
        }
        if (MATCH("GameState", "Player2"))
        {
            client_msg->game_state.player2_score = strtol(value, NULL, 0);
        }
        if (MATCH("GameState", "MaxRounds"))
        {
            client_msg->game_state.maxRoundsAmount = strtol(value, NULL, 0);
        }
        else
        {
            // error
        }
    }

    // if (strcmp(section, "PlayerTurn") == 0)
    // {
    //     client_msg->player_turn.isIncluded = true;
    //     if (MATCH("PlayerTurn", "Player"))
    //     {
    //         client_msg->player_turn.Player_no = strtol(value, NULL, 0);
    //     }
    //     if (MATCH("TuPlayerTurnrn", "Turn"))
    //     {
    //         strncpy(client_msg->player_turn.choice, value, sizeof(client_msg->player_turn.choice) - 1);
    //         client_msg->player_turn.choice[sizeof(client_msg->player_turn.choice) - 1] = '\0'; // Ensure null termination
    //     }
    //     else
    //     {
    //         // else
    //     }
    // }

    return 1;
}
