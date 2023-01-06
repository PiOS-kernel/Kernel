#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>
#include "heap.h"

uint8_t* alloc(size_t size);
void free(uint8_t* ptr, size_t size);

#endif