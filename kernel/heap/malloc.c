#include "malloc.h"

/* The great `malloc` */

uint8_t* malloc(size_t size) {
    return allocate_segment(&HEAP, size);
}