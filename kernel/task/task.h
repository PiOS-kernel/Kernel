#ifndef TASK_H
#define TASK_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// global variables
#define STACK_SIZE 4096
#define MIN_PRIORITY 10         //low values of prioriry represent the higher priority
struct TaskTCB *RUNNING = NULL;

// Definition of the Task Control Block.
// The struct's fields are stored in the order they appear in the definition:
// - bytes [0 - 3]: stp
// - bytes [4 - 7]: priority
// ... etc
typedef struct TaskTCB
{
    int* stp;                       // stack pointer
    int priority;                   // priority of the task
    int stack[STACK_SIZE];          // stack associated to the task
    struct TaskTCB* next;           // reference to the next Task_TCB
} TaskTCB;

typedef struct Queue
{
    struct TaskTCB* head;
    struct TaskTCB* tail;
}Queue;

//prioriy queues
Queue* WAITING_QUEUES[MIN_PRIORITY];
Queue* READY_QUEUES[MIN_PRIORITY];

bool empty (Queue* q);
TaskTCB* dequeue (Queue* q);
void enqueue (Queue* q, TaskTCB *task);
int count_tasks( Queue* q);
TaskTCB* schedule();
void memcpy(int* src, int* stp, int size);

#endif