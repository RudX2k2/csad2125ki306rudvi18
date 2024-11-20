#ifndef EMULATOR_PRIVATE_H
#define EMULATOR_PRIVATE_H

#include "emulator.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <time.h>



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
    EMULATOR_GAMESTATE_CLEAN_GAME,
} emulator_gamestate_enum_t;


typedef struct
{
    volatile emulator_gamestate_enum_t game_state;
    volatile uint8_t isLoaded : 1;
    volatile emulator_battlemode_enum_t cur_battlemode;
    uint8_t players_score[EMULATOR_PLAYERS_AMOUNT];
    volatile uint16_t max_rounds;

    emulator_player_type_enum_t players_type[EMULATOR_PLAYERS_AMOUNT];
    emulator_player_choice_enum_t players_choice[EMULATOR_PLAYERS_AMOUNT];
    volatile emulator_player_choice_enum_t recent_turn_choice;
    volatile emulator_players_enum_t winner;
    volatile uint8_t current_round;
} emulator_t;

static char emulator_turnresult_winner_strings[][20] =
    {
        "ROUND_WINNER_FIRST",
        "ROUND_WINNER_SECOND",
        "ROUND_WINNER_DRAW",
        "WINNER_DRAW",
        "WINNER_FIRST",
        "WINNER_SECOND",
};

static void EMULATOR_MainGameProccessThread(void *a);

#endif // EMULATOR_PRIVATE_H