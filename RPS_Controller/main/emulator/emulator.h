#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>

typedef enum{
    EMULATOR_ROCK = 0,
    EMULATOR_PAPER,
    EMULATOR_SCISSORS,
} emulator_player_choice_enum_t;

void EMULATOR_Init(void);

void EMULATOR_SetGameInfo(uint8_t isLoaded, char * mode, uint16_t player1Score, uint16_t player2Score, uint16_t maxRounds);

void EMULATOR_GiveSemIsTurnRetrieved(void);

int EMULATOR_TakeSemIsTurnRetrieved(void);

void EMULATOR_GiveSemIsStateRetrieved(void);

int EMULATOR_TakeSemIsStateRetrieved(void);

void EMULATOR_SetNewRecievedTurn(char * turn);

void EMULATOR_StartGame(void);

#endif // EMULATOR_H