#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SEGMENT_HEADER_SIZE sizeof(struct MemorySegment)
#define HEAP_SIZE 0x8000

/*
Memory Segments are the 'headers' of each memory block that is NOT YET allocated. 
That means that the heap is represented as as a linked list of available
memory segments.
*/

typedef struct MemorySegment {
    size_t size;
    struct MemorySegment* next;
} MemorySegment;

/*
The Heap simply contains a reference to the first available block of memory
*/

typedef struct Heap {
    MemorySegment* head;
} Heap;

/* The global heap */
static Heap HEAP;


const Heap new_heap();
void heap_init(Heap *heap, uint8_t* start_address, size_t size);
uint8_t* allocate_segment(Heap *heap, size_t size);
void free_segment(Heap *heap, uint8_t* start_address, size_t size);
void add_free_segment(Heap* heap, uint8_t* address, size_t size);
void heap_compaction(Heap* heap);
size_t available_space(Heap* heap);
size_t count_segments(Heap* heap);
MemorySegment* init_segment(uint8_t* start_address, size_t size);
uint8_t* segment_end_address(MemorySegment* seg);
void trim_segment(MemorySegment* seg, size_t target_size);

#endif