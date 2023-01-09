#include "../testing.h"
#include "../../kernel/task/task.h"
#include "../../kernel/heap/malloc.h"
#include "../../kernel/syscalls/syscalls.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool test_queue_empty() {
    Queue q;
    Queue_init(&q);
    ASSERT(empty(&q));
    ASSERT(count_tasks(&q) == 0);
    return true;
}

bool test_enqueue() {
    Queue q;
    Queue_init(&q);
    for (int i=0; i<5; i++) {
        TaskTCB* task = (TaskTCB*) alloc(sizeof(TaskTCB));

        enqueue(&q, task);
        ASSERT(!empty(&q));
        ASSERT(count_tasks(&q) == i + 1);
    }
    ASSERT(count_tasks(&q) == 5);

    return true;
}

bool test_dequeue() {
    Queue q;
    Queue_init(&q);
    for (int i=0; i<5; i++) {
        TaskTCB* task = (TaskTCB*) alloc(sizeof(TaskTCB));
        TaskTCB_init(task, i);

        enqueue(&q, task);
        ASSERT(!empty(&q));
        ASSERT(count_tasks(&q) == i + 1);
    }
    
    return true;
}

bool test_unlink_task() {
    Queue q;
    Queue_init(&q);

    TaskTCB* task = NULL;
    for (int i=0; i<5; i++) {
        task = (TaskTCB*) alloc(sizeof(TaskTCB));
        TaskTCB_init(task, i);

        enqueue(&q, task);
        ASSERT(!empty(&q));
        ASSERT(count_tasks(&q) == i + 1);
    }

    int i = 5;
    do {
        ASSERT(count_tasks(&q) == i--);
        TaskTCB* tmp = task->prev;
        unlink_task(task);
        task = tmp;
    } while (task != NULL);

    return true;
}

bool test_stack_push() {
    TaskTCB task;
    TaskTCB_init(&task, 0);
    uint8_t buffer[5] = {1, 2, 3, 4, 5};
    uint8_t* old_stp = task.stp;
    stack_push(&task, buffer, 5);

    for (int i=0; i<5; i++) {
        ASSERT(task.stp[i] == buffer[i]);
    }

    ASSERT(task.stp == old_stp - 5);
    return true;
}

bool test_stack_start() {
    TaskTCB task;
    TaskTCB_init(&task, 0);
    uint8_t* start = stack_start(&task);
    ASSERT(start == task.stack);
    return true;
}

bool test_stack_end() {
    TaskTCB task;
    TaskTCB_init(&task, 0);
    uint8_t* end = stack_end(&task);
    ASSERT(end == task.stack + STACK_SIZE);
    return true;
}

void task1(void* arg) {
    while(1);
}
void task2(void* arg) {
    while(1);
}
void task3(void* arg) {
    while(1);
}

bool test_schedule() {
    ASSERT(schedule() == NULL);

    kcreate_task(task1, NULL, 0);
    kcreate_task(task2, NULL, 1);
    kcreate_task(task3, NULL, 2);

    // Tasks 2 and 3 should never get the CPU, because they have lower
    // priorities than task 1.
    TaskTCB *t1 = schedule();
    ASSERT(t1 != NULL);
    ASSERT(t1->priority == 0);

    t1 = schedule();
    ASSERT(t1 != NULL);
    ASSERT(t1->priority == 0);

    t1 = schedule();
    ASSERT(t1 != NULL);
    ASSERT(t1->priority == 0);

    return true;
}

bool test_schedule1() {
    ASSERT(schedule() == NULL);

    kcreate_task(task1, NULL, 0);
    kcreate_task(task2, NULL, 1);
    kcreate_task(task3, NULL, 2);

    // First task to get the CPU is task 1.
    TaskTCB *t = schedule();
    ASSERT(t != NULL);
    ASSERT(t->priority == 0);
    // Task 1 is deleted.
    RUNNING = NULL;

    // Task 2 should get the CPU.
    t = schedule();
    ASSERT(t != NULL);
    ASSERT(t->priority == 1);
    // Task 2 is deleted.
    RUNNING = NULL;

    // Task 3 should get the CPU.
    t = schedule();
    ASSERT(t != NULL);
    ASSERT(t->priority == 2);

    return true;
}