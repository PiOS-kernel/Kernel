#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>
#include "heap.h"

#define free(ptr) if (ptr != NULL) {free_segment(&HEAP, (uint8_t*) ptr, sizeof(*ptr));}
uint8_t* malloc(size_t size);

#endif