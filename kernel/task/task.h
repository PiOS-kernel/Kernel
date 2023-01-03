#ifndef TASK_H
#define TASK_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// global variables
#define STACK_SIZE 4096
#define MIN_PRIORITY 10         //low values of prioriry represent the higher priority
extern struct TaskTCB *RUNNING;

// Definition of the Task Control Block.
// The struct's fields are stored in the order they appear in the definition:
// - bytes [0 - 3]: stp
// - bytes [4 - 7]: priority
// ... etc
typedef struct TaskTCB
{
    uint8_t* stp;                       // stack pointer
    uint8_t priority;                   // priority of the task
    uint8_t _word_alignment_filling[3]; // padding to align the next field on a 4-byte boundary
    uint8_t stack[STACK_SIZE];          // stack associated to the task
    struct TaskTCB* next;               // reference to the next Task_TCB in the list
    struct TaskTCB* prev;               // reference to the previous Task_TCB in the list
} TaskTCB;

typedef struct Queue
{
    TaskTCB* head;
    TaskTCB* tail;
}Queue;

//prioriy queues
extern Queue WAITING_QUEUES[MIN_PRIORITY];
extern Queue READY_QUEUES[MIN_PRIORITY];

// Task Control Block function prototypes
void TaskTCB_init(TaskTCB* tcb, uint8_t p);
uint8_t* stack_start(TaskTCB *task);
uint8_t* stack_end(TaskTCB *task);
void stack_push(TaskTCB * task, uint8_t* src, int size);
void unlink_task(TaskTCB *task);

// Queue function prototypes
void Queue_init(Queue* q);
bool empty (Queue* q);
TaskTCB* dequeue (Queue* q);
void enqueue (Queue* q, TaskTCB *task);
int count_tasks( Queue* q);
TaskTCB* schedule();

#endif