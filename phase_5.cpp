#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "AV_USART.h"
#include "AV_SPI.h"
#include "string_handling.h"

class vec3_t
{
public:
    int32_t x;
    int32_t y;
    int32_t z;
    vec3_t()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    vec3_t(int32_t initial)
    {
        x = initial;
        y = initial;
        z = initial;
    }
    vec3_t operator + (vec3_t in)
    {
        vec3_t out;
        out.x = x + in.x;
        out.y = y + in.y;
        out.z = z + in.z;
        return out;
    }
    void operator += (vec3_t in)
    {
        x + in.x;
        y + in.y;
        z + in.z;
    }
    vec3_t operator - (vec3_t in)
    {
        vec3_t out;
        out.x = x - in.x;
        out.y = y - in.y;
        out.z = z - in.z;
        return out;
    }
    void operator -= (vec3_t in)
    {
        x - in.x;
        y - in.y;
        z - in.z;
    }
    vec3_t operator * (vec3_t in)
    {
        vec3_t out;
        out.x = x * in.x;
        out.y = y * in.y;
        out.z = z * in.z;
        return out;
    }
    void operator *= (vec3_t in)
    {
        x * in.x;
        y * in.y;
        z * in.z;
    }
    vec3_t operator / (vec3_t in)
    {
        vec3_t out;
        out.x = x / in.x;
        out.y = y / in.y;
        out.z = z / in.z;
        return out;
    }
    void operator /= (vec3_t in)
    {
        x / in.x;
        y / in.y;
        z / in.z;
    }
    void operator = (vec3_t in)
    {
        x = in.x;
        y = in.y;
        z = in.z;
    }
};

vec3_t angular_rate;
vec3_t angular_rate_old;

vec3_t linear_acceleration;
vec3_t linear_acceleration_old;

vec3_t angle_accumulator;

vec3_t velocity_accumulator;

vec3_t position_accumulator;

vec3_t angle;

int16_t loops;

char output_data_register[64];

uint8_t index = 0;

void configure_control_timer()
{
    //set timer0 interrupt at 500 Hz
    // | COM0A 0-1 | COM0B 0-1 | - | - | WGM0 0-1 | 
    TCCR0A = 0b00000010;
    // | FOC0A | FOC0B | - | - | WGM0 2 | CS0 0-2 |
    TCCR0B = 0b00000100;
    // clear timer counter register
    TCNT0  = 0;
    // threshold value for 500 Hz interrupts
    OCR0A = 124;
    OCR0B = 0;
    // | - | - | - | - | - | OCIE0B | OCIE0A | TOIE0 |  
    TIMSK0 |= 0b00000010;
}

int main()
{
    //Set pin 8 as output
    DDRB |= 0b00000001;
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

    while (1)
    {
    
    }
    return 0;
}

ISR(TIMER0_COMPA_vect)
{
    

}




