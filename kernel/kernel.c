#include "kernel.h"

uint8_t HEAP_MEMORY[HEAP_SIZE];
Heap HEAP;
struct TaskTCB* RUNNING = NULL;
Queue WAITING_QUEUES[MIN_PRIORITY];
Queue READY_QUEUES[MIN_PRIORITY];

void kernel_init() {
    heap_init(&HEAP, HEAP_MEMORY, HEAP_SIZE);
}