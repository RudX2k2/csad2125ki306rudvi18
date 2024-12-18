#ifndef INI_HANDLER_PRIVATE_H
#define INI_HANDLER_PRIVATE_H

#include <string.h>
#include "ini_handler.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "iniparser/ini.h"
#include "../../uart_controller/include/uart_controller.h"
#include "../../emulator/include/emulator.h"
#include "../../common_data/emulator_commdata.h"
#define INIFILE_SIZE 512

// Game mode constants
#define GAME_MODE_PVP "PVP"
#define GAME_MODE_PVE "PVE"
#define GAME_MODE_EVE "EVE"

// Turn constants
#define TURN_ROCK "Rock"
#define TURN_PAPER "Paper"
#define TURN_SCISSORS "Scissors"

#define INIHNDLR_CMDBUFF_SIZE 2048
#define INIHNDLR_SEND_CLIENT_DATA_MAX_SIZE 100

typedef struct
{
    char command_buf[INIHNDLR_CMDBUFF_SIZE];
    size_t com_buf_size;
} INIHNDLR_t;

// Structure for game settings
typedef struct
{
    bool isIncluded;
    bool isLoaded : 1;
    GameState_CommonData_t gamestate;
} ClientMsg_SetGameConfig_t;

typedef struct
{
    bool isIncluded;
} ClientMsg_GetGameState_t;

typedef struct
{
    bool isIncluded;
    SetPlayerTurn_CommonData_t turn_result;
} ClientMsg_SetPlayerTurn_t;

typedef struct
{
    bool isIncluded;
} ClientMsg_CleanGame_t;


typedef struct
{
    ClientMsg_SetPlayerTurn_t set_player_turn;
    ClientMsg_SetGameConfig_t set_gameconfig;
    ClientMsg_GetGameState_t get_gamestate;
    ClientMsg_CleanGame_t clean_game;

} ClientMessage_t;

static void INIHANDLER_ParseTask(void *a);

static int INIHANDLER_ClientMessageParser(void *user, const char *section, const char *name, const char *value);

#endif // INI_HANDLER_PRIVATE_H
