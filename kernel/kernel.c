#include "kernel.h"

uint8_t HEAP_MEMORY[HEAP_SIZE];
Heap HEAP;
struct TaskTCB* RUNNING;
struct TaskTCB* IDLE_TASK;
uint32_t SHOULD_WAIT;
Queue READY_QUEUES[MIN_PRIORITY];
uint32_t CLOCK;

void _heap_init();
void _scheduler_init();

void kernel_init() {
    // Initialize the heap
    _heap_init();

    // Initialize scheduler data structures
    _scheduler_init();

    // The idle task is initialized
    idle_task_init();

    // Initialize the clock
    // The clock is incremented by sysTick after each quantum
    CLOCK = 0;
}

void _heap_init() {
    // Initialize the heap
    HEAP.head = NULL;
    heap_init(&HEAP, HEAP_MEMORY, HEAP_SIZE);
}

void _scheduler_init() {
    // Initialize scheduler data structures
    RUNNING = NULL;
    SHOULD_WAIT = 0;
    int i=0;
    for (; i<MIN_PRIORITY; i++) {
        Queue_init(&READY_QUEUES[i]);
    }
}