#ifndef INIHANDLER_H
#define INIHANDLER_H

#include <stdio.h>
#include "../common_data/emulator_commdata.h"
#include "../emulator/emulator.h"

/// @brief Read data form `rx.ini` file and perform operations
/// @param
void INIHANDLER_ParseCommand(char *buffer, uint32_t size);

// ------------------------- Send data to client ---------------------------

void INIHANDLER_SendWaitTurn(emulator_players_enum_t player);

void INIHANDLER_SendClientConfigResult(uint8_t result);

void INIHANDLER_SendClientCleanOk(void);

void INIHANDLER_GetTurnResult(GetTurnResult_CommonData_t turn_result);

void INIHANDLER_GetGameState(GameState_CommonData_t gamestate);

#endif // INIHANDLER_H
