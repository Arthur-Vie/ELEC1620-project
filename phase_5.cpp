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






