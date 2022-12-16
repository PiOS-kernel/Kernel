#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#define HEAP_SEG_HEADER_SIZE sizeof(struct HeapSegment)

typedef struct HeapSegment* SegmentLink;

struct HeapSegment {
    size_t size;
    SegmentLink next;
};

struct Heap {
    SegmentLink head;
};

struct LockedHeap {
    struct Mutex heap;
};

struct HeapIterator {
    SegmentLink next;
};

struct LockedHeap* LockedHeap_new(void) {
    struct LockedHeap* locked_heap = malloc(sizeof(struct LockedHeap));
    locked_heap->heap = Mutex_new();
    return locked_heap;
}

struct MutexGuard* LockedHeap_lock(const struct LockedHeap* locked_heap) {
    return Mutex_lock(&locked_heap->heap);
}

void LockedHeap_init(struct LockedHeap* locked_heap, size_t start_address, size_t size) {
    struct MutexGuard* guard = LockedHeap_lock(locked_heap);
    struct Heap* heap = guard->heap;
    Heap_add_free_segment(heap, start_address, size);
    MutexGuard_free(guard);
}

size_t LockedHeap_available_space(const struct LockedHeap* locked_heap) {
    struct MutexGuard* guard = LockedHeap_lock(locked_heap);
    struct Heap* heap = guard->heap;
    size_t available_space = Heap_available_space(heap);
    MutexGuard_free(guard);
    return available_space;
}

size_t LockedHeap_count_segments(const struct LockedHeap* locked_heap) {
    struct MutexGuard* guard = LockedHeap_lock(locked_heap);
    struct Heap* heap = guard->heap;
    size_t count_segments = Heap_count_segments(heap);
    MutexGuard_free(guard);
    return count_segments;
}

const Heap HEAP_NEW() {
    Heap heap;
    heap.head = NULL;
    return heap;
}

void HEAP_INIT(Heap *heap, size_t start_address, size_t size) {
    heap_add_free_segment(heap, start_address, size);
}

uint8_t *HEAP_ALLOCATE_SEGMENT(Heap *heap, size_t size) {
    if (heap->head == NULL) {
        return NULL;
    }

    size_t actual_size = (size > HEAP_SEG_HEADER_SIZE) ? size : HEAP_SEG_HEADER_SIZE;

    if (heap->head->size >= actual_size) {
        HeapSegment *old_head = heap->head;
        heap_trim_segment(old_head, actual_size);
        heap->head = old_head->next;
        return old_head->start_address;
    }

    HeapSegment *cursor = heap->head;
    bool advance = true;

    while (advance) {
        if (cursor->next == NULL) {
            return NULL;
        }
        advance = cursor->next->size < actual_size;
        if (advance) {
            cursor = cursor->next;
        }
    }

    HeapSegment *next = cursor->next;
    heap_trim_segment(next, actual_size);
    cursor->next = next->next;
    return next->start_address;
}

void HEAP_FREE_SEGMENT(Heap *heap, size_t start_address, size_t size) {
    heap_add_free_segment(heap, start_address, size);
    heap_compaction(heap);
}

void add_free_segment(Heap* self, size_t address, size_t size) {
    // The heap should never allocate segments of size less than
    // HEAP_SEG_HEADER_SIZE
    assert(size > HEAP_SEG_HEADER_SIZE);

    HeapSegment new_seg = init_segment(HeapSegment_new(size), address);
    if (self->head == NULL || self->head->start_address > address) {
        new_seg.next = self->head;
        self->head = &new_seg;
        return;
    }

    HeapSegment* cursor = self->head;
    bool advance = true;
    while (advance) {
        // Iterate through the list until a segment starting at a greater address 
        // than the new one is found

        advance = cursor->next != NULL && cursor->next->start_address < address;
        if (advance) {
            cursor = cursor->next;
        } else {
            // The segment is inserted into the list
            new_seg.next = cursor->next;
        }
    }
    cursor->next = &new_seg;
}

void compaction(Heap* self) {
    if (self->head == NULL) {
        return;
    }

    HeapSegment* cursor = self->head;
    while (true) {
        size_t node_start = cursor->start_address;
        bool compacted = cursor->next != NULL &&
            cursor->next->start_address == node_start + cursor->size;
        if (compacted) {
            cursor->size = cursor->size + cursor->next->size;
            cursor->next = cursor->next->next;
        } else {
            cursor = cursor->next;
        }
        if (cursor == NULL) {
            // The end of the list was reached, there are no more
            // segments to merge
            return;
        }
    }
}

size_t available_space(const Heap* self) {
    size_t total = 0;
    for (HeapSegment* seg = self->head; seg != NULL; seg = seg->next) {
        total += seg->size;
    }
    return total;
}

size_t count_segments(const Heap* self) {
    size_t total = 0;
    for (HeapSegment* seg = self->head; seg != NULL; seg = seg->next) {
        total += 1;
    }
    return total;
}

HeapSegment* init_segment(HeapSegment seg, size_t address) {
    HeapSegment* address_ptr = (HeapSegment*) address;
    *address_ptr = seg;
    return address_ptr;
}

void trim_segment(HeapSegment* seg, size_t target_size) {
    uintptr_t new_seg_addr = seg->start_address + target_size;
    size_t new_seg_size = seg->size - target_size;

    if (new_seg_size > HEAP_SEG_HEADER_SIZE) {
        seg->size = target_size;
        HeapSegment* new_seg = init_segment(malloc(sizeof(HeapSegment)), new_seg_addr, new_seg_size);
        new_seg->next = seg->next;
        seg->next = new_seg;
    }
}
