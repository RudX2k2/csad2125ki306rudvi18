#ifndef INIHANDLER_H
#define INIHANDLER_H

#include <stdio.h>

/// @brief Write retrieved buffer to `rx.ini` file so other functions can handle
/// @param buffer
/// @param size 
void INIHANDLER_WriteToINI_RX(char *buffer, uint64_t size);


/// @brief Read data form `rx.ini` file and perform operations
/// @param  
void INIHANDLER_ParseCommand(char * buffer, uint32_t size);


/// @brief Print `.ini` file
/// @param ini_file 
void INIHANDLER_PrintINI(const char * filename);


void INIHANDLER_SendWaitTurn(void);


#endif // INIHANDLER_H
