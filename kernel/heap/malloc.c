#include "malloc.h"

/* The great `malloc` */

char* malloc(size_t size) {
    return allocate_segment(&HEAP, size);
}