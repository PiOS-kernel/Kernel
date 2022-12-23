#ifndef KERNEL_H
#define KERNEL_H

#include "syscalls/syscalls.h"
#include "heap/malloc.h"
#include "task/task.h"

void kernel_init();
void start_scheduler();

#endif