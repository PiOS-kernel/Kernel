#include "kernel.h"

uint8_t HEAP_MEMORY[HEAP_SIZE];
Heap HEAP;
struct TaskTCB* RUNNING;
Queue READY_QUEUES[MIN_PRIORITY];

void kernel_init() {
    // Initialize the heap
    HEAP.head = NULL;
    heap_init(&HEAP, HEAP_MEMORY, HEAP_SIZE);

    // Initialize scheduler data structures
    RUNNING = NULL;
    int i=0;
    for (; i<MIN_PRIORITY; i++) {
        Queue_init(&READY_QUEUES[i]);
    }
}