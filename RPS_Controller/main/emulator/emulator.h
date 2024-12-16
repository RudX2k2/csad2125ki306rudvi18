#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include "../common_data/emulator_commdata.h"

typedef enum
{
    EMULATOR_CHOICE_ROCK = 0,
    EMULATOR_CHOICE_PAPER,
    EMULATOR_CHOICE_SCISSORS,
    EMULATOR_CHOICES_AMOUNT,
} emulator_player_choice_enum_t;

typedef enum
{
    EMULATOR_TURNRESULT_WINNER_ROUND_FIRST,
    EMULATOR_TURNRESULT_WINNER_ROUND_SECOND,
    EMULATOR_TURNRESULT_WINNER_ROUND_DRAW,
    EMULATOR_TURNRESULT_WINNER_DRAW,
    EMULATOR_TURNRESULT_WINNER_GAME_FIRST,
    EMULATOR_TURNRESULT_WINNER_GAME_SECOND,
} emulator_turnresult_winner_t;

typedef enum
{
    EMULATOR_BATTLEMODE_PVP = 0,
    EMULATOR_BATTLEMODE_PVE,
    EMULATOR_BATTLEMODE_EVE,
    EMULATOR_BATTLEMODE_IDLE,
    EMULATOR_BATTLEMODE_AMOUNT,
} emulator_battlemode_enum_t;

typedef enum
{
    EMULATOR_PLAYER_1 = 0,
    EMULATOR_PLAYER_2,
    EMULATOR_PLAYERS_AMOUNT,
} emulator_players_enum_t;

void EMULATOR_Init(void);

int EMULATOR_SetGameInfo(uint8_t isLoaded, char *mode, int player1Score, int player2Score, int curRound, int maxRounds);

GameState_CommonData_t EMULATOR_GetGameState(void);

void EMULATOR_GiveSemIsTurnRetrieved(void);

int EMULATOR_TakeSemIsTurnRetrieved(void);

void EMULATOR_GiveSemIsStateRetrieved(void);

int EMULATOR_TakeSemIsStateRetrieved(void);

void EMULATOR_SetNewRecievedTurn(char *turn);

void EMULATOR_StartGame(void);

/// @brief Takes singleton `emulator.player_choice` values and add score for the winner in `emulator.players_score`.
/// @return Result of the turn. Is it was last, and someone win - draw
emulator_turnresult_winner_t EMULATOR_ChoseWinner(void);

char *EMULATOR_GetBattleModeString(emulator_battlemode_enum_t mode);

void EMULATOR_CleanGame(void);

#endif // EMULATOR_H