#ifndef STRING_HANDLING_H
#define STRING_HANDLING_H

#include <stdint.h>

uint8_t int16_to_string_at_pointer(int16_t value, char * location);

uint8_t insert_at_pointer(char * text, uint8_t length, char * location);

char * int16_to_string(int16_t value);

#endif