#ifndef INIHANDLER_H
#define INIHANDLER_H

#include <stdio.h>
#include "../common_data/emulator_commdata.h"

/// @brief Write retrieved buffer to `rx.ini` file so other functions can handle
/// @param buffer
/// @param size
void INIHANDLER_WriteToINI_RX(char *buffer, uint64_t size);

/// @brief Read data form `rx.ini` file and perform operations
/// @param
void INIHANDLER_ParseCommand(char *buffer, uint32_t size);

/// @brief Print `.ini` file
/// @param ini_file
void INIHANDLER_PrintINI(const char *filename);

// ------------------------- Send data to client ---------------------------

void INIHANDLER_SendWaitTurn(void);

void INIHANDLER_SendClientGoodConfig(void);

void INIHANDLER_GetTurnResult(GetTurnResult_CommonData_t turn_result);

void INIHANDLER_GetGameState(GameState_CommonData_t gamestate);

#endif // INIHANDLER_H
