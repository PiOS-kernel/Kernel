#include "events.h"
#include "../kernel.h"


/* 
@brief: creates an event object and returns an handle.
*/

EventHandle new_event() {
    Event* event = alloc(sizeof(Event));

    // There is no handling of allocation failure because if no memory
    // is available, the system cannot be recovered to a valid state.

    Queue_init(&event->waiting_queue);

    return (EventHandle) event;
}

/*
@brief: the task is enqueued in the event waiting queue. All tasks in that
queue will be woken up when the event is signaled.

@param: event - the event handle
*/

void event_wait(EventHandle event) {
    Event* e = (Event*) event;

    // INTERRUPTS MUST BE DISABLED. Transactions to the shared waiting
    // queue must be atomic. Also the updating of the RUNNING pointer
    // must occur in the same atomic transaction.
    disable_interrupts();

    // The task is enqueued
    enqueue(&e->waiting_queue, RUNNING);

    // The RUNNING pointer is set to NULL, in order to prevent the scheduler
    // from inserting the running task in the READY queue.
    RUNNING = NULL;

    // Interrupts are enabled again
    enable_interrupts();

    // The scheduler is invoked
    PendSVTrigger(); // Should be a call to yield()
}

/*
@brief: the event is signaled and all tasks in the waiting queue are woken up.

@param: event - the event handle
*/

void event_post(EventHandle event) {
    Event* e = (Event*) event;

    // INTERRUPTS MUST BE DISABLED. Transactions to the shared running
    // queue must be atomic.
    disable_interrupts();

    // All tasks in the waiting queue are woken up
    while (!empty(&e->waiting_queue)) {
        TaskTCB* task = dequeue(&e->waiting_queue);
        enqueue(&READY_QUEUES[task->priority], task);
    }

    // Interrupts are enabled again
    enable_interrupts();
}
