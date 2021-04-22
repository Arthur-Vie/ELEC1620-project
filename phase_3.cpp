#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "AV_USART.h"
#include "AV_SPI.h"

char int16_string[7];

uint16_t abs(int16_t value)
{
    if (value < 0)
    {
        return -(uint16_t)value;
    }
    else
    {
        return (uint16_t)value;
    }
}

char * signed16_to_string(int16_t value)
{
    uint8_t index = 0;
    if (value < 0)
    {
        int16_string[index] = '-';
        index ++;
    }
    uint16_t abs_value;
    abs_value = abs(value);
    if (abs_value == 0)
    {
        int16_string[index] = '0';
        index ++;
    }
    else
    {
        uint8_t counter = 0;
        uint8_t i;
        while (abs_value)
        {
            for (i = 0; i < counter; i++)
            {
                int16_string[index - i] = int16_string[index - i - 1];
            }
            int16_string[index - i] = (abs_value % 10) + 48;
            abs_value /= 10;
            index ++;
            counter ++;
        }
    }
    int16_string[index] = '\0';
    return int16_string;
}

int main()
{
    // baud rates 115200 and above cause corrupt data. Noise on USB link?
    USART_init(16000000, 19200);
    SPI_master_init();
    if (SPI_read_register(0x0F) == 'h')
    {
        USART_transmit_multi_bytes((uint8_t *)("Communication with LSM9DS1 working.\n\r"), 37);
    }
    else
    {
       USART_transmit_multi_bytes((uint8_t *)("Failed to communicate with LSM9DS1.\n\r"), 37); 
    }

    char * test_string = "This is a test\n\r\0";
    USART_transmit_string(test_string);
    int16_t test_number = -12345;
    USART_transmit_string(signed16_to_string(test_number));
    USART_newline();

    int16_t i = 0;

    while (1)
    {
        USART_transmit_string(signed16_to_string(i));
        USART_newline();

        i ++;

        _delay_ms(100);
    }
    return 0;
}