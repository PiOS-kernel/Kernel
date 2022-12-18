#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include "heap.h"

#define free(ptr) if (ptr != NULL) {add_free_segment(&HEAP, (char*) ptr, sizeof(*ptr));}
char* malloc(size_t size);

#endif