#ifndef KERNEL_H
#define KERNEL_H

#include "syscalls/syscalls.h"
#include "heap/malloc.h"
#include "task/task.h"
#include "synch/semaphores.h"
#include "synch/events.h"
#include "synch/channels.h"

void kernel_init();
void start_scheduler();
uint32_t get_clock();

#endif