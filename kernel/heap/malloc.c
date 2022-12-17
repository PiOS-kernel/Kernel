#include "heap.h"

/* The great `malloc` */

char* malloc(size_t size) {
    return allocate_segment(&HEAP, size);
}

/* The great free, as a macro */

#define free(ptr) if (ptr != NULL) {add_free_segment(ptr, sizeof(*ptr));}