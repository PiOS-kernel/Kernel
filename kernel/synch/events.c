#include "events.h"
#include "../kernel.h"
#include "../utils/utils.h"

/** 
 * @brief: creates an event object and returns an handle.

 * @param: msg_size - the size of the message.
 * @return: an handle to the event object.
*/

EventHandle new_event(uint32_t msg_size) {
    Event* event = (Event*) alloc(sizeof(Event) + msg_size);

    // There is no handling of allocation failure because if no memory
    // is available, the system cannot be recovered to a valid state.

    // The message that comes with the event is stored just after the
    // event object itself. This is done in order to minimize heap
    // fragmentation.
    // This implementation detail is hidden from the application. The
    // application will only access the messagge through the dedicated
    // API.
    event->msg_ptr = (void*) ((uint32_t) event + sizeof(Event));
    event->msg_size = msg_size;
    Queue_init(&event->waiting_queue);

    return (EventHandle) event;
}

/**
 * @brief: the task is enqueued in the event waiting queue. All tasks in the 
 * queue will be woken up when the event is signaled.

 * @param: event - the event handle
*/

void event_wait(EventHandle event) {
    Event* e = (Event*) event;

    // INTERRUPTS MUST BE DISABLED. Transactions to the shared waiting
    // queue must be atomic.
    disable_interrupts();

    // The task is enqueued
    enqueue(&e->waiting_queue, RUNNING);

    // Tells the scheduler that the task should not be inserted back in
    // the ready queue
    SHOULD_WAIT = 1;

    // Interrupts are enabled again
    enable_interrupts();

    // The scheduler is invoked
    yield();
}

/**
 * @brief: the event is signaled and all tasks in the waiting queue are woken up.

 * @param: event - the event handle
 * @param: msg_ptr - the message that comes with the event
*/

void event_post(EventHandle event, void* msg_ptr) {
    Event* e = (Event*) event;

    // INTERRUPTS MUST BE DISABLED. Transactions to the shared running
    // queue must be atomic.
    disable_interrupts();

    memcpy((uint8_t*) msg_ptr, (uint8_t*) e->msg_ptr, e->msg_size);

    // All tasks in the waiting queue are woken up
    while (!empty(&e->waiting_queue)) {
        TaskTCB* task = dequeue(&e->waiting_queue);
        enqueue(&READY_QUEUES[task->priority], task);
    }

    // Interrupts are enabled again
    enable_interrupts();
}

/**
 * @brief: the event message is copied to the destination buffer.

 * @param: event - the event handle
 * @param: dst - the destination buffer
*/

void get_event_msg(EventHandle event, void* dst) {
    Event* e = (Event*) event;
    memcpy((uint8_t*) e->msg_ptr, (uint8_t*) dst, e->msg_size);
}

/**
 * @brief: the function cleans up memory allocated for the event structures.

 * @param: event - the event handle
*/

void delete_event(EventHandle event) {
    // This function will be implemented once dynamic memory de-allocation
    // will be fixed.
    ;
}