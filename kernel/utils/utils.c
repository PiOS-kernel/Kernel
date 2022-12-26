/* The great memcpy */

#include "utils.h"
#include "stddef.h"

void memcpy(uint8_t* src, uint8_t* dst, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        *(dst+i) = *(src+i);
    }
}

void memset(uint8_t* str, uint8_t c, size_t n)
{
    uint8_t *s = (uint8_t *)str;
    while (n--)
        *s++ = (uint8_t)c;
}