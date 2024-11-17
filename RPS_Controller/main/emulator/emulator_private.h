#ifndef EMULATOR_PRIVATE_H
#define EMULATOR_PRIVATE_H

#include "emulator.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <time.h>


typedef enum
{
    EMULATOR_PLAYER_1 = 0,
    EMULATOR_PLAYER_2,
    EMULATOR_PLAYERS_AMOUNT,
} emulator_players_enum_t;

typedef enum
{
    EMULATOR_PLAYER_TYPE_HUMAN = 0,
    EMULATOR_PLAYER_TYPE_BOT,
} emulator_player_type_enum_t;

typedef enum
{
    EMULATOR_GAMESTATE_WAIT_CONFIG = 0,
    EMULATOR_GAMESTATE_WAIT_PLAYER1_TURN,
    EMULATOR_GAMESTATE_WAIT_PLAYER2_TURN,
    EMULATOR_GAMESTATE_PROCCESS_PLAYERS_GAME,
    EMULATOR_GAMESTATE_PROCCESS_PLAYERBOT_GAME,
    EMULATOR_GAMESTATE_PROCCESS_BOTS_GAME,
    EMULATOR_GAMESTATE_SHOW_WINNER,
} emulator_gamestate_enum_t;


typedef struct
{
    emulator_gamestate_enum_t game_state;
    uint8_t isLoaded : 1;
    emulator_battlemode_enum_t cur_battlemode;
    uint8_t players_score[EMULATOR_PLAYERS_AMOUNT];
    uint16_t max_rounds;

    emulator_player_type_enum_t players_type[EMULATOR_PLAYERS_AMOUNT];
    emulator_player_choice_enum_t players_choice[EMULATOR_PLAYERS_AMOUNT];
    emulator_player_choice_enum_t recent_turn_choice;
    emulator_players_enum_t winner;
} emulator_t;

static void EMULATOR_MainGameProccessThread(void *a);

#endif // EMULATOR_PRIVATE_H