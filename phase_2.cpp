#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "AV_USART.h"
#include "AV_SPI.h"

int main()
{
    // baud rates 115200 and above cause corrupt data. Noise on USB link?
    USART_init(16000000, 19200);
    SPI_master_init();
    USART_transmit_multi_bytes((uint8_t *)("START\n\r"), 7);

    while (1)
    {
        // Transmit the contents of the WHO_AM_I register at 0x0F
        USART_transmit_byte(SPI_read_register(0x0F));
        USART_transmit_byte('\n');
        USART_transmit_byte('\r');
        _delay_ms(1000);
    }
    return 0;
}