#include "task.h"
#include "../utils/utils.h"
#include <stddef.h>

// create an instance of a TaskTCB with priority p
void TaskTCB_init(TaskTCB* tcb, uint8_t p)
{
    tcb->priority = p;
    tcb->next = NULL;
    // The stack pointer is initialized to the end address of the task's stack
    tcb->stp = (uint8_t*) (tcb->stack + STACK_SIZE);
}

// utility method that computes the start address of the stack
uint8_t* stack_start(TaskTCB *task)
{
    return (uint8_t*) task->stack;
}

// utility method that computes the end address of the stack
uint8_t* stack_end(TaskTCB *task)
{
    uint8_t* end = (uint8_t*) task->stack + STACK_SIZE; 
    return end;
}

// utility method to push values onto the task's stack
void stack_push(TaskTCB * task, uint8_t* src, int size)
{
    // Check whether there is room left on the stack
    if (task->stp - size > stack_start(task))
    {
        // The stack pointer is decremented and data is pushed onto the stack
        task->stp = task->stp - size;
        memcpy(src, task->stp, size);   
    }
}

// initialize the queue with both head and tail NULL
void Queue_init(Queue* q)
{
    q->head = NULL;
    q->tail = NULL;
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
            old_head->next = NULL;
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
    while (counter != NULL)
    {
        count++;
        counter = counter->next;
    }
    return count;
}

//return the higer priority task ready to be executed 
TaskTCB* schedule() 
{
    TaskTCB *selected = NULL;

    // get the priority of the running task
    int running_priority = MIN_PRIORITY - 1;
    if (RUNNING != NULL)
        running_priority = RUNNING->priority;

    // look for a task in the ready queues with a priority higher or equal
    // to the priority of the running task
    int i;
    for (i=0; i<=running_priority; i++)
    {
        if (!empty(&READY_QUEUES[i]))
        {
            selected = dequeue(&READY_QUEUES[i]);
            break;
        }
    }

    // If there was a running task, and the scheduler has selected
    // a new task to be executed, the previously running task is
    // inserted back in the ready queue
    if (RUNNING != NULL && selected != NULL) 
        enqueue(&READY_QUEUES[RUNNING->priority], RUNNING);
    
    if (selected != NULL) {
        RUNNING = selected; // set the selected task as the running task
        return selected;
    }
    return RUNNING;
}
