#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

void memcpy(uint8_t* src, uint8_t* dst, int size);
void memset(uint8_t* str, uint8_t c, size_t n);

#endif