/* The great memcpy */

#include "utils.h"
#include "stddef.h"

void memcpy(uint8_t* src, uint8_t* dst, int size)
{
    for (int i = 0; i < size; i++)
    {
        *(dst+i) = *(src+i);
    }
}

void memset(uint8_t* str, uint8_t c, size_t n)
{
    unsigned char *s = (unsigned char *)str;
    while (n--)
        *s++ = (unsigned char)c;
}