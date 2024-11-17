#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include "../common_data/emulator_commdata.h"

typedef enum{
    EMULATOR_ROCK = 0,
    EMULATOR_PAPER,
    EMULATOR_SCISSORS,
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
} emulator_battlemode_enum_t;


typedef enum
{
    EMULATOR_PLAYER_1 = 0,
    EMULATOR_PLAYER_2,
    EMULATOR_PLAYERS_AMOUNT,
} emulator_players_enum_t;

void EMULATOR_Init(void);

void EMULATOR_SetGameInfo(uint8_t isLoaded, char * mode, uint16_t player1Score, uint16_t player2Score, uint16_t maxRounds);

GameState_CommonData_t EMULATOR_GetGameState(void);

void EMULATOR_GiveSemIsTurnRetrieved(void);

int EMULATOR_TakeSemIsTurnRetrieved(void);

void EMULATOR_GiveSemIsStateRetrieved(void);

int EMULATOR_TakeSemIsStateRetrieved(void);

void EMULATOR_SetNewRecievedTurn(char * turn);

void EMULATOR_StartGame(void);


/// @brief Takes singleton `emulator.player_choice` values and add score for the winner in `emulator.players_score`.
/// @return Result of the turn. Is it was last, and someone win - draw
emulator_turnresult_winner_t EMULATOR_ChoseWinner();

char *EMULATOR_GetBattleModeString(emulator_battlemode_enum_t mode);

#endif // EMULATOR_H