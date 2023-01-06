#ifndef SYSTASKS_H
#define SYSTASKS_H

#include <stdint.h>
#include <stddef.h>

void create_task(void (*code)(void *), void* args, uint8_t priority);
void exit();
void yield();
void kcreate_task(void (*code)(void *), void *args, uint8_t priority);
void task_switch();

#endif