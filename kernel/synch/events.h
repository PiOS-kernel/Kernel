#ifndef EVENTS_H
#define EVENTS_H

#include "../task/task.h"
#include "../exceptions.h"

typedef struct Event {
    Queue waiting_queue;
} Event;

typedef void* EventHandle;

EventHandle new_event();
void event_wait(EventHandle event);
void event_post(EventHandle event);

#endif