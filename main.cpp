#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "AV_USART.h"

uint8_t words[15] = "Hello World!\n\r";

int main()
{
    DDRB = 0xFF;
    // baud rates 115200 and above cause corrupt data. Noise on USB link?
    USART_init(16000000, 19200);

    while (1)
    {
        PORTB = 0xFF;
        USART_transmit_multi_bytes(words, 15);
        _delay_ms(500);
        PORTB = 0;
        _delay_ms(500);
    }
    return 0;
}


