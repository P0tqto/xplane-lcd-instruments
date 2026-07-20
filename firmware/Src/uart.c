#include "uart.h"

void UART2_Init(void) {

    // enable clock to GPIOA and USART2
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    RCC->APB1ENR |= (RCC_APB1ENR_USART2EN);
    
    // configure PA2 (TX) and PA3 (RX) to AF (Alternate Function mode)
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);

    // map PA2 and PA3 to AF7 (USART2)
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3);
    GPIOA->AFR[0] |= (7U << GPIO_AFRL_AFSEL2_Pos | (7U << GPIO_AFRL_AFSEL3_Pos)); // 2, 3 times 4 cus each pin gets 4 bit in AFR register 

    // set baud rate
    USART2->BRR = 0x8B;

    // enable USART2 (bit 13), transmitter (bit 3), receiver (bit 2)
    USART2->CR1 |= ((1 << 13) | (1 << 3) | (1 << 2));
}


char UART2_Read(void) {
    while (!(USART2->SR & (1 << 5)));
    return (char)(USART2->DR & 0xFF);
}
