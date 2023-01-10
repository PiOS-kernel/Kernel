#include "kernel.h"

uint8_t HEAP_MEMORY[HEAP_SIZE];
Heap HEAP;
struct TaskTCB* RUNNING;
uint32_t SHOULD_WAIT;
Queue READY_QUEUES[MIN_PRIORITY];
uint32_t CLOCK;

void kernel_init() {
    // Initialize the heap
    HEAP.head = NULL;
    heap_init(&HEAP, HEAP_MEMORY, HEAP_SIZE);

    // Initialize scheduler data structures
    RUNNING = NULL;
    SHOULD_WAIT = 0;
    int i=0;
    for (; i<MIN_PRIORITY; i++) {
        Queue_init(&READY_QUEUES[i]);
    }

    // Initialize the clock
    // The clock is incremented by sysTick after each quantum
    CLOCK = 0;
}