#ifndef EVENTS_H
#define EVENTS_H

#include "../task/task.h"
#include "../exceptions.h"
#include <stdint.h>

#define NEW_EVENT(msg_type) new_event(sizeof(msg_type))
#define NEW_EVENT_NO_MSG() new_event(0)

/*
An event object is a synchronization primitive that allows tasks to wait for
an event to occur. When the event is signaled, for example from an ISR, all
tasks that are waiting for the event are woken up.
*/

typedef struct Event {
    Queue waiting_queue;
    void* msg_ptr;
    uint32_t msg_size;
} Event;

typedef void* EventHandle;

EventHandle new_event(uint32_t msg_size);
void event_wait(EventHandle event);
void event_post(EventHandle event, void* msg_ptr);
void get_event_msg(EventHandle event, void* dst);
void delete_event(EventHandle event);

#endif