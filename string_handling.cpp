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