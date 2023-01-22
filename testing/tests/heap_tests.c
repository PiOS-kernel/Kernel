#include "../testing.h"
#include "../../kernel/heap/heap.h"
#include "../../kernel/heap/malloc.h"

bool test_heap_init() {
    uint8_t heap_mem[1024];
    Heap heap = new_heap();
    heap_init(&heap, (uint8_t*) heap_mem, 1024);
    
    ASSERT(available_space(&heap) == 1024);
    ASSERT(count_segments(&heap) == 1);
    return true;
}

bool test_heap_alignment() {
    uint8_t heap_mem[1024];
    Heap heap = new_heap();
    heap_init(&heap, (uint8_t*) heap_mem, 1024);
    
    for (int i=0; i<10; ++i) {
        bool* ptr = (bool*) alloc(sizeof(bool));
        ASSERT(((uint32_t) ptr) % 4 == 0);
    }
    return true;
}

bool test_heap_alignment1() {
    uint8_t heap_mem[1024];

    // The heap start address is not word-aligned
    if ( ((uint32_t) heap_mem) % 4 != 0 ) {
        Heap heap = new_heap();
        heap_init(&heap, (uint8_t*) heap_mem, 512);

        ASSERT(heap.head->size != 1024);
        ASSERT(((uint32_t) &(heap.head)) % 4 == 0);
    } 
    // The heap is word-aligned
    else {
        Heap heap = new_heap();
        heap_init(&heap, (uint8_t*) heap_mem + 1, 512);

        ASSERT(heap.head->size != 1024);
        ASSERT(((uint32_t) &(heap.head)) % 4 == 0);
    }
    return true;
}

bool test_count_segments() {
    uint8_t heap_mem[1024];
    Heap heap = new_heap();
    
    for (int i=0; i<25; ++i) {
        uint8_t* base_address = (uint8_t*) heap_mem + i * 40;
        add_free_segment(&heap, base_address, 40);
        ASSERT(count_segments(&heap) == i+1);
    }
    return true;
}

bool test_available_space() {
    uint8_t heap_mem[1024];
    Heap heap = new_heap();
    heap_init(&heap, (uint8_t*) heap_mem, 1024);
    
    for (int i=0; i<25; ++i) {
        allocate_segment(&heap, 40);
        ASSERT(available_space(&heap) == 1024 - (i+1) * 40);
    }
    return true;
}

bool test_heap_compaction() {
    uint8_t heap_mem[1024];
    Heap heap = new_heap();
    
    add_free_segment(&heap, (uint8_t*) heap_mem, 128);
    ASSERT(available_space(&heap) == 128);
    ASSERT(count_segments(&heap) == 1);

    add_free_segment(&heap, (uint8_t*) heap_mem + 128, 128);
    ASSERT(available_space(&heap) == 256);
    ASSERT(count_segments(&heap) == 2);
    
    heap_compaction(&heap);
    ASSERT(available_space(&heap) == 256);
    ASSERT(count_segments(&heap) == 1);
    return true;
}

bool test_heap_compaction1() {
    uint8_t heap_mem[1024];
    Heap heap = new_heap();

    add_free_segment(&heap, (uint8_t*) heap_mem, 256);
    ASSERT(available_space(&heap) == 256);
    ASSERT(count_segments(&heap) == 1);

    add_free_segment(&heap, (uint8_t*) heap_mem + 256, 256);
    ASSERT(available_space(&heap) == 512);
    ASSERT(count_segments(&heap) == 2);

    add_free_segment(&heap, (uint8_t*) heap_mem + 512, 256);
    ASSERT(available_space(&heap) == 768);
    ASSERT(count_segments(&heap) == 3);

    add_free_segment(&heap, (uint8_t*) heap_mem + 768, 256);
    ASSERT(available_space(&heap) == 1024);
    ASSERT(count_segments(&heap) == 4);

    heap_compaction(&heap);
    ASSERT(available_space(&heap) == 1024);
    ASSERT(count_segments(&heap) == 1);

    return true;
}

bool test_allocate_segment() {
    uint8_t heap_mem[1024];
    Heap heap = new_heap();
    heap_init(&heap, (uint8_t*) heap_mem, 1024);
    
    // Segments are allocated

    uint8_t* segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 896);
    ASSERT(count_segments(&heap) == 1);
    
    segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 768);
    ASSERT(count_segments(&heap) == 1);
    
    segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 640);
    ASSERT(count_segments(&heap) == 1);
    
    uint8_t* segment2 = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 512);
    ASSERT(count_segments(&heap) == 1);

    // Free to segments, and observe heap compaction taking place
    free_segment(&heap, segment, 128);
    ASSERT(available_space(&heap) == 640);
    ASSERT(count_segments(&heap) == 2);

    free_segment(&heap, segment2, 128);
    ASSERT(available_space(&heap) == 768);
    ASSERT(count_segments(&heap) == 1);

    // Start allocating segments again
    segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 640);
    ASSERT(count_segments(&heap) == 1);

    segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 512);
    ASSERT(count_segments(&heap) == 1);

    segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 384);
    ASSERT(count_segments(&heap) == 1);
    
    segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 256);
    ASSERT(count_segments(&heap) == 1);
    
    segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 128);
    ASSERT(count_segments(&heap) == 1);
    
    segment = allocate_segment(&heap, 128);
    ASSERT(segment != NULL);
    ASSERT(available_space(&heap) == 0);
    ASSERT(count_segments(&heap) == 0);

    // Since there is no more space, the next allocation should fail
    // and a NULL pointer should be returned
    
    segment = allocate_segment(&heap, 128);
    ASSERT(segment == NULL);
    ASSERT(available_space(&heap) == 0);
    ASSERT(count_segments(&heap) == 0);
    
    return true;
}

bool test_allocate_segment1() {
    // The minimum amount of memory allocated must be equal
    // to SEGMENT_HEADER_SIZE
    uint8_t* ptr = (uint8_t*) alloc(sizeof(uint8_t));
    ASSERT(ptr != NULL);
    ASSERT(available_space(&HEAP) == HEAP_SIZE - SEGMENT_HEADER_SIZE);

    mem_free(ptr, sizeof(uint8_t));
    ASSERT(available_space(&HEAP) == HEAP_SIZE);
    return true;
}

bool test_trim_segment() {
    // Decrease the size of the first segment by just 1 byte
    trim_segment(HEAP.head, HEAP_SIZE - 1);
    ASSERT(count_segments(&HEAP) == 1);
    ASSERT(HEAP.head->size == HEAP_SIZE);

    // Run compaction
    heap_compaction(&HEAP);
    ASSERT(count_segments(&HEAP) == 1);

    // Now decrease the size of the segment to 128 bytes
    trim_segment(HEAP.head, 128);
    ASSERT(count_segments(&HEAP) == 2);
    ASSERT(HEAP.head->size == 128);
    ASSERT(HEAP.head->next->size == HEAP_SIZE - 128);

    return true;
}