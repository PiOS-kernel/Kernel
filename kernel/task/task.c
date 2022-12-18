#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "task.h"

// create an instance of a TaskTCB with priority p
TaskTCB new_TaskTCB( uint8_t p )
{
    TaskTCB new_task;
    new_task.priority = p;
    new_task.next = NULL;
    // The stack pointer is initialized to the start address of the task's stak
    new_task.stp = new_task.stack;
    return new_task;
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

// initialize the queue with both head and tail NULL
struct Queue *new_Queue()
{
    struct Queue *queue = malloc(sizeof(struct Queue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

// return true if the queue is empty
bool empty( Queue* q)
{
    return q->head == NULL;
}

// enqueue a TaskTCB at the end of the queue
void enqueue (Queue* q, TaskTCB *task)
{
    if (empty(q))
    {
        // if the queue is empty add the element in the head
        q->head = task;
        q->tail = task;
    }
    else
    {
        // if it is not empty enqueue the element as the last elemnt
        q->tail->next = task;
        q->tail = q->tail->next; // update the tail to the new end of the queue
    }
}

// dequque the first element of the queue
TaskTCB* dequeue (Queue* q){
    if (!empty(q))
    {
        TaskTCB *old_head = q->head;
        if (old_head->next != NULL)
        {
            // shift the head to the following element in the queue 
            q->head = old_head->next; 
        }
        else
        {
            q->head = NULL;
            q->tail = NULL;
        }
        return old_head; // return the popped element
    }
    else
    {
        return NULL; // return NULL if the queue was already empty
    }
}

// returns the number of tasks currently in the queue
int count_tasks( Queue* q)
{
    int count = 0;
    TaskTCB *counter = q->head;
    while (counter->next != NULL)
    {
        count++;
        counter = counter->next;
    }
    return count;
}

//return the higer priority task ready to be executed 
TaskTCB* schedule() 
{
    TaskTCB *selected;
    //look for the first element of the higer priority queue which is not empty
    for (int i=0; i<MIN_PRIORITY; i++)
    {
        if (!empty(READY_QUEUES[i]))
        {
            selected = dequeue(READY_QUEUES[i]);
            break;
        }
    }
    return NULL; //return NULL if all of the ready queues are empty 
}