#include "malloc.h"

/**
 * @brief: Memory allocation primitive for a PiOS application
 * @param: the amount of memory to be allocated
 * @return: a pointer to the allocated memory
*/

uint8_t* alloc(size_t size) {
    return allocate_segment(&HEAP, size);
}

/**
 * @brief: De-allocation primitive 
 * 
 * @param: the memory to be de-allocated 
 * @param: the amount of memory to be de-allocated
*/

void free(uint8_t* ptr, size_t size) {
    // The minimum amount of memory allocated is equal to SEGMENT_HEADER_SIZE
    if (size < SEGMENT_HEADER_SIZE)
        size = SEGMENT_HEADER_SIZE;
    
    // Memory is allocated to 4-byte boundaries
    size = (size + 3) & ~3;

    free_segment(&HEAP, ptr, size);
}
