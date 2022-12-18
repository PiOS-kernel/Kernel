/* The great memcpy */

#include "utils.h"

void memcpy(uint8_t* src, uint8_t* stp, int size);
{
    for (int i = 0; i < size; i++)
    {
        *(dest+i) = *(src+i);
    }
}