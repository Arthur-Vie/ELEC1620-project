#include <stdint.h>

#ifndef HEAP_SIZE
#pragma GCC warning "Heap size has not been specified. Using 1024 bytes by default.\n You can specify any heap size up to 65536 by writing \"#define HEAP_SIZE\" at the top of your program"
#define HEAP_SIZE 1024
#endif

uint8_t byte_heap[HEAP_SIZE];

uint8_t * base_pointer = byte_heap;
uint8_t * end_pointer = byte_heap + (HEAP_SIZE - 1);
uint16_t bytes_available = HEAP_SIZE;
uint16_t current_heap_top = 0;
uint16_t bit_stack = 0;

void heap_clear()
{
    uint16_t i;
    for (i = 0; i < HEAP_SIZE; i ++)
    {
        byte_heap[i] = 0;
    }
}

void heap_init()
{
    heap_clear();
}

uint8_t * alloc_bytes(uint16_t num_bytes)
{
    uint16_t pointer_offset = 0;
    int8_t iterator = 1;
    uint16_t index;
    uint8_t bit;
    uint16_t counter = 0;
    uint8_t write_flag = 0;

    for (index = (HEAP_SIZE - 1); index > (current_heap_top + num_bytes); index -= iterator)
    {
        for ((iterator == 1)?(bit = 0):(bit = 8); ((iterator == 1)?(bit < 8):(bit > 0)); bit += iterator)
        {
            if (((byte_heap[index] >> bit) & 0b00000001) == 0)
            {
                counter += iterator;
                if (write_flag && (counter <= num_bytes))
                {
                    if (counter == 0)
                    {
                        current_heap_top += num_bytes;
                        return base_pointer + pointer_offset;
                    }
                    byte_heap[index] |= (1 << (bit - 1));
                }
            }
            else
            {
                pointer_offset ++;
                counter = 0;
            }
            if (counter == (num_bytes + 2))
            {
                iterator = -1;
                write = 1;
            }
        }
    }
}

void free_bytes(uint8_t * heap_pointer, uint16_t num_bytes)
{

}