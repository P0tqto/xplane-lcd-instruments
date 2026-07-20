#include "stm32f4xx.h"
#include "lcd.h"
#include "uart.h"

volatile uint32_t msTicks = 0; 

extern void SysTick_Handler(void)
{
    if (msTicks != 0)
    {
        msTicks--;
    }
}

void delay_ms(uint32_t ms)
{
    msTicks = ms;
    while (msTicks != 0)
    {
    }
}

int main(void)
{
    // initialize clock and timing engine
    SysTick_Config(SystemCoreClock / 1000);

    // initialize the screen
    LCD_Init();
    UART2_Init();

    // set up template
    LCD_SendCommand(0x01);             // clear screen and wait 
    delay_ms(2);
    LCD_SendCommand(0x80);             // line 1 on LCD screen (datasheet)
    LCD_SendString("ALT:        FT"); 
    LCD_SendCommand(0xC0);             // line 2
    LCD_SendString("VS:         FPM"); 
    char data_buffer[6]; 
    int buffer_idx = 0;
    char current_mode = '\0'; 

    while (1)
    {
        char rx_byte = UART2_Read(); 

        // search for valid packet prefix
        if (current_mode == '\0')
        {
            if (rx_byte == 'A' || rx_byte == 'V')
            {
                current_mode = rx_byte; 
                buffer_idx = 0;      
            }
        }

        // retrieve and process sent package, should be 5 characters each
        else
        {
            data_buffer[buffer_idx] = rx_byte;
            buffer_idx++;

            // when there's 5 characters, process the string
            if (buffer_idx == 5)
            {
                data_buffer[5] = '\0'; // force string termination in case UART payload was truncated

                if (current_mode == 'A')
                {
                    LCD_SendCommand(0x85); // move cursor right after "ALT: "
                    LCD_SendString(data_buffer);
                }
                else if (current_mode == 'V')
                {
                    LCD_SendCommand(0xC5); // move cursor right after "VS: "
                    LCD_SendString(data_buffer);
                }

                // reset the process to wait for the next packet prefix ('A' or 'V')
                current_mode = '\0';
            }
        }
    }
}