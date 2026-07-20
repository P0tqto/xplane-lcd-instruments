#ifndef UART_H_
#define UART_H_

#include "stm32f4xx.h" 

void UART2_Init(void);
char UART2_Read(void);
void UART2_ReadPacket(char* buffer, int length); 

#endif