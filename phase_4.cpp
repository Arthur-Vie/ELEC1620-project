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
    else
    {
        int16_string[index] = '+';
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

    // Write control reg 1
    // 952 Hz ODR, 500 dps full scale, 100 Hz bw
    SPI_write_register(0x10, 0b11001011);
    // Write control reg 2
    // defaults
    //SPI_write_register(0x11, 0b00000000);
    // Write control reg 3
    // defaults
    // SPI_write_register(0x12, 0b00000000);
    // Write control reg 4
    // Enable gyroscope X, Y and Z
    SPI_write_register(0x1E, 0b00111000);
    // Write control reg 5
    // Enable linear accelerometer X, Y and Z
    SPI_write_register(0x1F, 0b00111000);
    // Write control reg 6
    // 952 Hz ODR, +- 4g full scale
    SPI_write_register(0x20, 0b11010000);
    // Write control reg 7
    // defaults
    //SPI_write_register(0x21, 0b00000000);
    // Write control reg 8
    // Enable block data update and auto address increment
    SPI_write_register(0x22, 0b01000100);
    // Write control reg 9
    // Disable I2C
    SPI_write_register(0x23, 0b00000100);
    // Write control reg 10
    // defaults
    //SPI_write_register(0x24, 0b00000000);

    int16_t angular_rate_X = 0;
    int16_t angular_rate_X_old = 0;
    int16_t angular_rate_Y = 0;
    int16_t angular_rate_Y_old = 0;
    int16_t angular_rate_Z = 0;
    int16_t angular_rate_Z_old = 0;

    int16_t linear_acceleration_X = 0;
    int16_t linear_acceleration_X_old = 0;
    int16_t linear_acceleration_Y = 0;
    int16_t linear_acceleration_Y_old = 0;
    int16_t linear_acceleration_Z = 0;
    int16_t linear_acceleration_Z_old = 0;

    while (1)
    {
        // Collect the lower and upper byte of the X axis angular rate measurement from they gyroscope
        angular_rate_X = 0 | SPI_read_register(0x18);
        angular_rate_X |= (SPI_read_register(0x19) << 8);
        // Collect the lower and upper byte of the Y axis angular rate measurement from they gyroscope
        angular_rate_Y = 0 | SPI_read_register(0x1A);
        angular_rate_Y |= (SPI_read_register(0x1B) << 8);
        // Collect the lower and upper byte of the Z axis angular rate measurement from they gyroscope
        angular_rate_Z = 0 | SPI_read_register(0x1C);
        angular_rate_Z |= (SPI_read_register(0x1D) << 8);

        // Collect the lower and upper byte of the X axis linear acceleration measurement from they gyroscope
        linear_acceleration_X = 0 | SPI_read_register(0x28);
        linear_acceleration_X |= (SPI_read_register(0x29) << 8);
        // Collect the lower and upper byte of the Y axis linear acceleration measurement from they gyroscope
        linear_acceleration_Y = 0 | SPI_read_register(0x2A);
        linear_acceleration_Y |= (SPI_read_register(0x2B) << 8);
        // Collect the lower and upper byte of the Z axis linear acceleration measurement from they gyroscope
        linear_acceleration_Z = 0 | SPI_read_register(0x2C);
        linear_acceleration_Z |= (SPI_read_register(0x2D) << 8);


        
        USART_transmit_string(signed16_to_string(angular_rate_X - angular_rate_X_old));
        USART_transmit_multi_bytes((uint8_t *)(",\t"), 2);
        USART_transmit_string(signed16_to_string(angular_rate_Y - angular_rate_Y_old));
        USART_transmit_multi_bytes((uint8_t *)(",\t"), 2);
        USART_transmit_string(signed16_to_string(angular_rate_Z - angular_rate_Z_old));
        USART_transmit_multi_bytes((uint8_t *)(",\t"), 2);
        USART_transmit_string(signed16_to_string(linear_acceleration_X - linear_acceleration_X_old));
        USART_transmit_multi_bytes((uint8_t *)(",\t"), 2);
        USART_transmit_string(signed16_to_string(linear_acceleration_Y - linear_acceleration_Y_old));
        USART_transmit_multi_bytes((uint8_t *)(",\t"), 2);
        USART_transmit_string(signed16_to_string(linear_acceleration_Z - linear_acceleration_Z_old));
        USART_transmit_byte(';');
        USART_newline();

        angular_rate_X_old = angular_rate_X;
        angular_rate_Y_old = angular_rate_Y;
        angular_rate_Z_old = angular_rate_Z;

        linear_acceleration_X_old = linear_acceleration_X;
        linear_acceleration_Y_old = linear_acceleration_Y;
        linear_acceleration_Z_old = linear_acceleration_Z;

        _delay_ms(50);
    }
    return 0;
}