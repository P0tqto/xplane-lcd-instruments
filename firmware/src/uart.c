#include "uart.h"

static volatile char rx_buf[UART_RX_BUF_SIZE];
static volatile uint8_t rx_head = 0; // written by ISR
static volatile uint8_t rx_tail = 0; // read by main loop

void UART2_Init(void)
{

    // enable clock to GPIOA and USART2
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    RCC->APB1ENR |= (RCC_APB1ENR_USART2EN);

    // configure PA2 (TX) and PA3 (RX) to AF (Alternate Function mode)
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);

    // map PA2 and PA3 to AF7 (USART2)
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3);
    GPIOA->AFR[0] |= (7U << GPIO_AFRL_AFSEL2_Pos | (7U << GPIO_AFRL_AFSEL3_Pos));

    // set baud rate
    USART2->BRR = 0x8B;

    // enable RXNE interrupt, USART2 (bit 13), transmitter (bit 3), receiver (bit 2)
    USART2->CR1 |= USART_CR1_RXNEIE;
    USART2->CR1 |= ((1 << 13) | (1 << 3) | (1 << 2));

    // enable USART2 interrupt line in NVIC
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 1);
}

void USART2_IRQHandler(void)
{
    if (USART2->SR & USART_SR_RXNE)
    {
        char byte = (char)(USART2->DR & 0xFF); // reading DR also clears RXNE
        uint8_t next_head = (rx_head + 1) % UART_RX_BUF_SIZE;

        if (next_head != rx_tail) // only store if buffer isn't full
        {
            rx_buf[rx_head] = byte;
            rx_head = next_head;
        }
    }
}

uint8_t UART2_ReadByte(char* byte) {
    if (rx_head == rx_tail)
    {
        return 0; 
    }
    *byte = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % UART_RX_BUF_SIZE;
    return 1;
}
