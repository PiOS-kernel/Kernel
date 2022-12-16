#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 4096
#define MAX_PRIORITY 10

typedef struct TaskTCB *TcbBlock;

// Definition of the Task Control Block.
// The struct's fields are stored in the order they appear in the definition:
// - bytes [0 - 3]: stp
// - bytes [4 - 7]: priority
// ... etc
struct TaskTCB
{
    uint8_t *stp;              // stack pointer
    uint8_t priority;          // priority of the task
    uint8_t stack[STACK_SIZE]; // stack associated to the task
    TcbBlock next;             // reference to the next Task_TCB
};

// global variables
struct TaskTCB *RUNNING = NULL;

// constructor for a TaskTCB that return an instance of a TaskTCB
// with the associating the parameters to the corresponding fields
struct TaskTCB *new_TaskTCB(TcbBlock n, uint8_t p)
{
    struct TaskTCB *tcb = malloc(sizeof(struct TaskTCB));
    tcb->next = n;
    tcb->priority = p;
    tcb->stp = NULL;
    memset(tcb->stack, 0, STACK_SIZE);

    // The stack pointer is initialized to the start address of the task's
    // stack
    tcb->stp = &tcb->stack[STACK_SIZE];
    return tcb;
}

// utility method that computes the start address of the stack
uint8_t *stack_start(struct TaskTCB *self)
{
    return &self->stack[0];
}

// utility method that computes the end address of the stack
uint8_t *stack_end(struct TaskTCB *self)
{
    return &self->stack[STACK_SIZE];
}

// utility method to push values onto the task's stack
void stack_push(struct TaskTCB *self, uint8_t *src, size_t size)
{
    // Check whether there is room left on the stack
    if ((uintptr_t)self->stp < (uintptr_t)&self->stack[0])
    {
        exit(-1); // execution is halted
    }

    // The data is stored onto the stack and the stack pointer
    // is decremented.
    self->stp = self->stp - size;
    memcpy(src, self->stp, size);
}

typedef struct TaskTCB *TcbBlock;

struct TaskTCB
{
    uint8_t *stp;              // stack pointer
    uint8_t priority;          // priority of the task
    uint8_t stack[STACK_SIZE]; // stack associated to the task
    TcbBlock next;             // reference to the next Task_TCB
};

// This type implements the Iterator trait, which allows to iterate through
// a Queue through the for task in queue construct
struct QueueIterator
{
    TcbBlock next;
};

struct Queue
{
    TcbBlock head;
    struct TaskTCB *tail;
};

// initialize the queue with both head and tail NULL
struct Queue *new_Queue()
{
    struct Queue *queue = malloc(sizeof(struct Queue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

// return true if the queue is empty
bool empty(struct Queue *self)
{
    return self->head == NULL;
}

// returns an iterator
struct QueueIterator *iter(struct Queue *self)
{
    struct QueueIterator *iterator = malloc(sizeof(struct QueueIterator));
    iterator->next = self->head;
    return iterator;
}

// enqueue a TaskTCB at the end of the queue
void enqueue(struct Queue *self, struct TaskTCB *block)
{
    struct TaskTCB *tail_ptr = block; // create raw pointer to the new element just created

    if (empty(self))
    {
        // if the queue is empty add the element in the head
        self->head = block;
    }
    else
    {
        // if it is not empty add the elemente in the tail.next
        self->tail->next = block;
    }
    self->tail = tail_ptr; // update the tail to the new end of the queue
}

// dequque the first element of the queue
struct TaskTCB *dequeue(struct Queue *self)
{
    if (self->head != NULL)
    {
        struct TaskTCB *old_head = self->head;
        if (old_head->next != NULL)
        {
            // shift the head to the current head.next and update the tail if
            self->head = old_head->next; // it is the last element
        }
        else
        {
            self->head = NULL;
            self->tail = NULL;
        }
        return old_head; // return the popped element
    }
    else
    {
        return NULL; // return NULL if the queue was already empty
    }
}

// returns the number of tasks currently in the queue
size_t count_tasks(struct Queue *self)
{
    size_t count = 0;
    struct QueueIterator *iterator = iter(self);
    while (iterator->next != NULL)
    {
        count += 1;
        iterator->next = iterator->next->next;
    }
    free(iterator);
    return count;
}


Queue WAITING_QUEUE;
atomic_int RUNNING;

TaskTCB* schedule() {
    if (!WAITING_QUEUE.empty()) {
        TaskTCB* tcb = WAITING_QUEUE.dequeue().unwrap();
        RUNNING = (int) tcb;
        return tcb;
    } else {
        if (RUNNING) {
            return (TaskTCB*) RUNNING;
        } else {
            return NULL;
        }
    }
}

typedef struct QueueIterator {
    TaskTCB* next;
} QueueIterator;

TaskTCB* next(QueueIterator* iter) {
    if (iter->next) {
        TaskTCB* node = iter->next;
        iter->next = iter->next->next;
        return node;
    } else {
        return NULL;
    }
}