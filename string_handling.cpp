#include <stdint.h>

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

uint8_t int16_to_string_at_pointer(int16_t value, char * location)
{
    uint8_t index = 0;
    if (value < 0)
    {
        location[index] = '-';
        index ++;
    }
    else
    {
        location[index] = '+';
        index ++;
    }
    uint16_t abs_value;
    abs_value = abs(value);
    if (abs_value == 0)
    {
        location[index] = '0';
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
                location[index - i] = location[index - i - 1];
            }
            location[index - i] = (abs_value % 10) + 48;
            abs_value /= 10;
            index ++;
            counter ++;
        }
    }
    return index;
}

uint8_t insert_at_pointer(char * text, uint8_t length, char * location)
{
    uint8_t index = 0;
    while (index < length)
    {
        location[index] = text[index];
        index ++;
    }
    return index;
}

char * int16_to_string(int16_t value)
{
    uint8_t index = int16_to_string_at_pointer(value, int16_string);
    int16_string[index] = '\0';
    return int16_string;
}