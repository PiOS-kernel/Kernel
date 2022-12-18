/* The great memcpy */

#include "utils.h"

void memcpy(uint8_t* src, uint8_t* dst, int size)
{
    for (int i = 0; i < size; i++)
    {
        *(dst+i) = *(src+i);
    }
}