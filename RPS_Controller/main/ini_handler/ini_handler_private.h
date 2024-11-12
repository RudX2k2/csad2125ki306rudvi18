#ifndef INI_HANDLER_PRIVATE_H
#define INI_HANDLER_PRIVATE_H

#include <stdio.h>
#include <string.h>

#include "ini_handler.h"

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "iniparser/ini.h"
#include "../uart_controller/uart_controller.h"
#include "../emulator/emulator.h"

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

typedef struct
{
    char command_buf[INIHNDLR_CMDBUFF_SIZE];
    size_t com_buf_size;
} INIHNDLR_t;

// [Client]
// [GameState]
// Retrieve=1
// Mode=
// Player1=
// Player2=
// MaxRounds=

// Structure for game settings
typedef struct
{
    bool isIncluded;
    uint8_t isGameToLoad : 1;
    char mode[4]; // PVP, PVE, EVE
    uint16_t player1_score;
    uint16_t player2_score;
    uint8_t maxRoundsAmount;
} ClientMsg_GameState_t;


typedef struct
{
    bool isIncluded;
    uint8_t Player_no;
    char choice[10];
} ClientMsg_PlayerTurn;

typedef struct
{
    bool client_found;
    ClientMsg_GameState_t game_state;
    ClientMsg_PlayerTurn player_turn;
} ClientMessage_t;

static void INIHANDLER_ParseTask(void *a);

static int INIHANDLER_ClientMessageParser(void *user, const char *section, const char *name, const char *value);

#endif // INI_HANDLER_PRIVATE_H
