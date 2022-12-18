#ifndef SYSTASKS_H
#define SYSTASKS_H

#include <stdint.h>
#include <stddef.h>

void create_task(void (*code)(void *), void* args, uint32_t priority);
void kcreate_task(void (*code)(void *), void *args, size_t priority);
void task_switch();

#endif