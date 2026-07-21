#ifndef LCD_H
#define LCD_H
#define LCD_D4_PIN 3

#include "stm32f4xx.h"

void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *str);

#endif