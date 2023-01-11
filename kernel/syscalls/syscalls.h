#ifndef SYSTASKS_H
#define SYSTASKS_H

#include <stdint.h>
#include <stddef.h>

typedef void* TaskHandle;

TaskHandle create_task(void (*code)(void *), void* args, uint8_t priority);
void exit();
void yield();
void kill(TaskHandle task);
TaskHandle kcreate_task(void (*code)(void *), void *args, uint8_t priority);
void task_switch();
void pre_context_switch() __attribute__((weak));

#endif