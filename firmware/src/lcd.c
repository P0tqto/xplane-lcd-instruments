#include "lcd.h"

static void LCD_SendNibble(uint8_t nibble);
static void LCD_PulseEnable(void);
extern void delay_ms(uint32_t ms);

void LCD_GPIO_Init(void)
{
    // enable clock to GPIOA and GPIOB
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);

    // set PA0-1 to general purpose output mode
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1);
    GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0);

    // set PB3-6 to general purpose output mode
    GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 |
                      GPIO_MODER_MODER5 | GPIO_MODER_MODER6);
    GPIOB->MODER |= (GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 |
                     GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0);

    // start pins at low state (0V)
    GPIOA->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR1); 
    GPIOB->BSRR = (GPIO_BSRR_BR3 | GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR6);
}

// Tell LCD to read data
void LCD_PulseEnable(void)
{
    GPIOA->ODR |= (1 << 1); // Set E (PA1) High

    // The HD44780 chip needs the pulse to stay high for at least 450 nanoseconds.
    // At 84MHz or 100MHz, a tiny loop or a few NOPs is plenty of time.
    for (volatile int i = 0; i < 50; i++)
        ;

    GPIOA->ODR &= ~(1 << 1); // Set E (PA1) Low (Falling Edge triggers read)

    // Wait for the command to be registered inside the screen
    for (volatile int i = 0; i < 1000; i++)
        ;
}

void LCD_SendNibble(uint8_t nibble)
{
    // clear only bits 3 to 6 in Port B without wiping other pins
    GPIOB->ODR &= ~(0xF << 3);

    // take the bottom 4 bits of 'nibble', shift them up to bit 3, and write
    GPIOB->ODR |= ((nibble & 0x0F) << 3);

    LCD_PulseEnable();
}

void LCD_SendCommand(uint8_t cmd)
{
    GPIOA->BSRR = GPIO_BSRR_BR0; // RS = 0 (Instruction Register Mode)

    // send upper 4 bits
    LCD_SendNibble(cmd >> 4);

    // send lower 4 bits
    LCD_SendNibble(cmd & 0x0F);
}

void LCD_SendData(uint8_t data)
{
    GPIOA->BSRR = GPIO_BSRR_BS0; // RS = 1 (Data Register Mode)

    // send upper 4 bits
    LCD_SendNibble(data >> 4);

    // send lower 4 bits
    LCD_SendNibble(data & 0x0F);
}

void LCD_Init(void)
{
    // initialize pins and clocks
    LCD_GPIO_Init();

    // wait for screen to boot
    delay_ms(50);

    // enter command mode / instruction register (RS = 0 on datasheet)
    GPIOA->BSRR = GPIO_BSRR_BS0;

    // wake up sequence
    LCD_SendNibble(0x03);
    delay_ms(5); // wait > 4.1ms (datasheet)

    LCD_SendNibble(0x03);
    delay_ms(1); // wait > 100us

    LCD_SendNibble(0x03);
    delay_ms(1);

    // officially lock the screen into 4-bit data mode
    LCD_SendNibble(0x02);
    delay_ms(1);

    // settings
    // function Set: 4-bit mode, 2 display lines, 5x8 font size
    LCD_SendCommand(0x28);

    // display Control: turn display ON, turn cursor/blink OFF
    LCD_SendCommand(0x0C);

    // clear the screen completely
    LCD_SendCommand(0x01);
    delay_ms(2); // Clearing the display takes the screen ~1.52ms to process
}

void LCD_SendString(char *str)
{
    while (*str != '\0')
    {
        LCD_SendData((uint8_t)(*str)); // send the current character
        str++;                         // advance the pointer to the next character
    }
}