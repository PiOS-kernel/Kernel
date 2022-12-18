#ifndef SYSTASKS_H
#define SYSTASKS_H

#include <stdint.h>
#include <stddef.h>

typedef void(fn_ptr)(void*);
void create_task(fn_ptr code, void* args, uint32_t priority);
void kcreate_task(void (*code)(void *), void *args, size_t priority);
void task_switch();

#endif