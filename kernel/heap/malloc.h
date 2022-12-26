#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>
#include "heap.h"

#define free(ptr) if (ptr != NULL) {free_segment(&HEAP, (uint8_t*) ptr, sizeof(*ptr) > SEGMENT_HEADER_SIZE ? sizeof(*ptr) : SEGMENT_HEADER_SIZE);}
uint8_t* alloc(size_t size);

#endif