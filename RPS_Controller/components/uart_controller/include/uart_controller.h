#pragma once
#include <stdint.h>

void UARTCNTRL_Init(void);

void UARTCNTRL_SendData(char *buffer, uint32_t size);

void UARTCNTRL_EnableRXDataPolling(void);
