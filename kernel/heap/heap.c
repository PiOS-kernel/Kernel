#include "heap.h"

const Heap new_heap() {
    Heap heap;
    heap.head = NULL;
    return heap;
}

/* Initializes the heap as a single empty memory block */

void heap_init(Heap *heap, uint8_t* start_address, size_t size) {
    add_free_segment(heap, start_address, size);
}

/* 
Allocates to a memory segment of the given size, and returns a pointer to it
to the caller.
*/

uint8_t* allocate_segment(Heap *heap, size_t size) {
    // There is no available memory left
    if (heap->head == NULL) {
        return NULL;
    }

    // The heap never allocates segments smaller than SEGMENT_HEADER_SIZE
    size_t actual_size = (size > SEGMENT_HEADER_SIZE) ? size : SEGMENT_HEADER_SIZE;

    // Check the head first
    if (heap->head->size >= actual_size) {
        MemorySegment *old_head = heap->head;

        // The segment is split into two new ones, and the firt one is 
        // allocated
        trim_segment(old_head, actual_size);
        heap->head = old_head->next;
        return (uint8_t*) old_head;
    }
    
    // Iterates through the list until a fitting free segment is found
    MemorySegment* current = heap->head;
    MemorySegment* previous = NULL;

    while (current->size < actual_size) {   // Safe pointer dereference
        previous = current;
        current = current->next;

        // No fitting segment is found
        if (current == NULL) return NULL;
    }

    // The segment is split into two new ones. One of the required size, which
    // is allocated, and the other of the remaining size, which is inserted again
    // in the linked list;
    trim_segment(current, actual_size);
    
    // The allocated segment is removed from the linked list
    if (previous != NULL) {
        previous->next = current->next;
    } else heap->head = current->next;
    
    return (uint8_t*) current;
}

/* 
When a segment is freed, it is put back into the list of
free segments
*/

void free_segment(Heap *heap, uint8_t* start_address, size_t size) {
    add_free_segment(heap, start_address, size);

    // Ajacent segments are merged
    heap_compaction(heap);
}

/* 
The functions inserts a segment into the free list. Segments are stored in the
linked list so that if segment A comes after segment B in the linked list, the
memory address where segment A lives comes after the memory address where segment
B lives. This allows to efficiently merge together adjecent segments
*/

void add_free_segment(Heap* heap, uint8_t* address, size_t size) {
    // The header is written in memory
    MemorySegment* new_seg = init_segment(address, size);

    // The segment might be inserted on top of the list
    if (heap->head == NULL || (uint8_t*) heap->head > address) {
        new_seg->next = heap->head;
        heap->head = new_seg;
        return;
    }

    // Iterate through the list until a segment starting at a greater address 
    // than the new one is found
    MemorySegment* cursor = heap->head;
    while (cursor->next != NULL && (uint8_t*) cursor->next < address)
        cursor = cursor->next;

    // The segment is inserted into the list
    new_seg->next = cursor->next;
    cursor->next = new_seg;
}

/*
The function looks for adjecent segments and merges them into a single one
*/

void heap_compaction(Heap* self) {
    // There is no available memory in the heap
    if (self->head == NULL) {
        return;
    }

    MemorySegment* cursor = self->head;

    // Adjecent segments are pairs of segments where the end address of the first
    // one is equal to the start address of the second one. 
    // As long as adjecent segments are found, they are merged into a single one.
    while (cursor->next != NULL) {
        if (segment_end_address(cursor) == (uint8_t*) cursor->next) {
            cursor->size = cursor->size + cursor->next->size;
            cursor->next = cursor->next->next;
        } else {
            cursor = cursor->next;
        }
    }
}

/* Utility function to compute the total available space in the heap */

size_t available_space(Heap* heap) {
    size_t total = 0;
    for (MemorySegment* seg = heap->head; seg != NULL; seg = seg->next) {
        total += seg->size;
    }
    return total;
}

/* Utility function that returns the number of free segments in the heap */

size_t count_segments(Heap* heap) {
    size_t total = 0;
    for (MemorySegment* seg = heap->head; seg != NULL; seg = seg->next) {
        total += 1;
    }
    return total;
}

/*
The function is given a memory address where it should initialize a 
`MemorySegment` struct. It returns a pointer to it
*/

MemorySegment* init_segment(uint8_t* start_address, size_t size) {
    MemorySegment* seg_ptr = (MemorySegment*) start_address;
    seg_ptr->size = size;
    seg_ptr->next = NULL;
    return seg_ptr;
}

/* Utility function that computes the memory address where the given segment ends */

uint8_t* segment_end_address(MemorySegment* seg) {
    return ((uint8_t*) seg) + seg->size;
}

/*
The function trims down a segment splitting it into two new ones of sizes
<target_size> and <size - target_size>
*/

void trim_segment(MemorySegment* seg, size_t target_size) {
    uint8_t* new_seg_addr = ((uint8_t*) seg) + target_size;
    size_t new_seg_size = seg->size - target_size;

    if (new_seg_size > SEGMENT_HEADER_SIZE) {
        seg->size = target_size;
        MemorySegment* new_seg = init_segment(new_seg_addr, new_seg_size);
        new_seg->next = seg->next;
        seg->next = new_seg;
    }
}
