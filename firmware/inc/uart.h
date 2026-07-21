#ifndef UART_H_
#define UART_H_
#define UART_RX_BUF_SIZE 32

#include "stm32f4xx.h" 

void UART2_Init(void);
uint8_t UART2_ReadByte(char *byte);

#endif