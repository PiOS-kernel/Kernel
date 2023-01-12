#ifndef SYSTASKS_H
#define SYSTASKS_H

#include <stdint.h>
#include <stddef.h>

typedef void* TaskHandle;

void create_task(void (*code)(void *), void* args, uint8_t priority, TaskHandle* handle);
void task_exit();
void yield();
void kill(TaskHandle task);
void kcreate_task(void (*code)(void *), void *args, uint8_t priority, TaskHandle* handle);
void task_switch();
void pre_context_switch() __attribute__((weak));
TaskHandle get_my_taskHandle()

#endif