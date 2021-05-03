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
            x += in.x;
            y += in.y;
            z += in.z;
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
            x -= in.x;
            y -= in.y;
            z -= in.z;
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
            x *= in.x;
            y *= in.y;
            z *= in.z;
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
            x /= in.x;
            y /= in.y;
            z /= in.z;
        }
        void operator = (vec3_t in)
        {
            x = in.x;
            y = in.y;
            z = in.z;
        }
};

class vec3_16_t: public vec3_t
{
    public:

        int16_t x;
        int16_t y;
        int16_t z;
};

class vec3_32_t: public vec3_t
{
    public:

        int32_t x;
        int32_t y;
        int32_t z;
};

class angle_t
{
    private:
        // Maximum value of the accumulator for a full 360 degree rotation
        // at 500 dps full scale (16-bit integer values)
        #define ACCUMULATOR_MAX 11796120
        int16_t update_loops(int32_t * accumulator, int32_t value)
        {

            *accumulator = *accumulator + value;
            if ((*accumulator) >= ACCUMULATOR_MAX)
            {
                *accumulator = *accumulator - ACCUMULATOR_MAX;
                return 1;
            }
            else if ((*accumulator) < 0)
            {
                *accumulator = *accumulator + ACCUMULATOR_MAX;
                return -1;
            }
            else
            {
                return 0;
            }
        }

    public:

        vec3_t accumulator;

        struct degrees
        {
            int16_t x;
            int16_t y;
            int16_t z;
        } degrees;
        
        struct loops
        {
            int16_t x;
            int16_t y;
            int16_t z;
        } loops;

        angle_t()
        {
            degrees.x = 0;
            degrees.y = 0;
            degrees.z = 0;
            loops.x = 0;
            loops.y = 0;
            loops.z = 0;
        }
        void update(vec3_t in)
        {
            // Factor for converting the accumulator value to a value in degrees
            // given settings of 500 dps full scale and 0.002 second sampling period.
            #define ANGLE_SCALE_FACTOR 32767
            loops.x += update_loops(&(accumulator.x), in.x);
            loops.y += update_loops(&(accumulator.y), in.y);
            loops.z += update_loops(&(accumulator.z), in.z);
            degrees.x = (int16_t)(accumulator.x / ANGLE_SCALE_FACTOR);
            degrees.y = (int16_t)(accumulator.y / ANGLE_SCALE_FACTOR);
            degrees.z = (int16_t)(accumulator.z / ANGLE_SCALE_FACTOR);
        }
};

class distance_t
{
    private:

        int16_t update_component(int16_t * component, int16_t update)
        {
            (*component) = update;
            if ((*component) >= 1000)
            {
                (*component) -= 1000;
                return 1;
            }
            else if ((*component) < 0)
            {
                (*component) += 1000;
                return -1;
            }
            else
            {
                return 0;
            }
        }

    public:

        struct distance_mm
        {
            int16_t x;
            int16_t y;
            int16_t z;
        } mm;
        
        struct distance_m
        {
            int16_t x;
            int16_t y;
            int16_t z;
        } m;

        distance_t()
        {
            mm.x = 0;
            mm.y = 0;
            mm.z = 0;
            m.x = 0;
            m.y = 0;
            m.z = 0;
        }
        void update(vec3_t in)
        {
            m.x += update_component(&mm.x, (int16_t)in.x);
            m.y += update_component(&mm.y, (int16_t)in.y);
            m.z += update_component(&mm.z, (int16_t)in.z);
        }
};

vec3_t angular_rate;
vec3_t angular_rate_old = -340;

vec3_t current_angular_rate;

vec3_t linear_acceleration;
vec3_t linear_acceleration_old;

vec3_t velocity_accumulator;

vec3_t position_accumulator;

angle_t angle;

distance_t position;

char output_data_register[92];

uint8_t bytes_left = 0;

uint8_t top = 0;

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

    configure_control_timer();

    sei();

    while (1)
    {
    
    }
    return 0;
}

ISR(TIMER0_COMPA_vect)
{
    //Raise pin 8
    PORTB |= 0b00000001;
    // Collect the lower and upper byte of the X axis angular rate measurement from they gyroscope
    angular_rate.x = 0 | SPI_read_register(0x18);
    angular_rate.x |= (SPI_read_register(0x19) << 8);
    // Collect the lower and upper byte of the Y axis angular rate measurement from they gyroscope
    angular_rate.y = 0 | SPI_read_register(0x1A);
    angular_rate.y |= (SPI_read_register(0x1B) << 8);
    // Collect the lower and upper byte of the Z axis angular rate measurement from they gyroscope
    angular_rate.z = 0 | SPI_read_register(0x1C);
    angular_rate.z |= (SPI_read_register(0x1D) << 8);

    // Collect the lower and upper byte of the X axis linear acceleration measurement from they gyroscope
    linear_acceleration.x = 0 | SPI_read_register(0x28);
    linear_acceleration.x |= (SPI_read_register(0x29) << 8);
    // Collect the lower and upper byte of the Y axis linear acceleration measurement from they gyroscope
    linear_acceleration.y = 0 | SPI_read_register(0x2A);
    linear_acceleration.y |= (SPI_read_register(0x2B) << 8);
    // Collect the lower and upper byte of the Z axis linear acceleration measurement from they gyroscope
    linear_acceleration.z = 0 | SPI_read_register(0x2C);
    linear_acceleration.z |= (SPI_read_register(0x2D) << 8);

    velocity_accumulator += (linear_acceleration - linear_acceleration_old);

    position_accumulator += velocity_accumulator;

    current_angular_rate += (angular_rate - angular_rate_old);

    angle.update(current_angular_rate);  //angle_accumulator / (vec3_t)32767);

    position.update((vec3_t)0);//position_accumulator / (vec3_t)208760);

    if (bytes_left == 0){
        bytes_left += int16_to_string_at_pointer(angle.degrees.x, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(angle.loops.x, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(angle.degrees.y, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(angle.loops.y, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(angle.degrees.z, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(angle.loops.z, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(position.mm.x, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(position.m.x, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(position.mm.y, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(position.m.y, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(position.mm.z, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(",\t", 2, output_data_register + bytes_left);
        bytes_left += int16_to_string_at_pointer(position.m.z, output_data_register + bytes_left);
        bytes_left += insert_at_pointer(";\n\r", 3, output_data_register + bytes_left);

        top = bytes_left;
    }
    else
    {
        USART_transmit_byte(output_data_register[top - bytes_left]);
        bytes_left --;
    }

    angular_rate_old = angular_rate;

    linear_acceleration_old = linear_acceleration;

    PORTB &= 0b11111110;

}




