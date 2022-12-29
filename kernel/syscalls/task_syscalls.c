#include "syscalls.h"
#include "../task/task.h"
#include "../heap/malloc.h"
#include "../utils/utils.h"
#include "../exceptions.h"
#include <stddef.h>
#include <stdint.h>

/*
This is the system call provided to the user application, in order to
create a new task.
It accepts a function pointer, a pointer to its arguments, and a priority.
The function simply invokes the kernel to request the given service.
*/

extern void create_task(void (*code)(void *), void* args, uint8_t priority);

/*
This is the system call provided to the user application to terminate execution
of the current task.
*/

extern void exit();

/*

Kernel space implementation for create_task(), brief description:
This is the function used by the kernel to create a new task
The functions pushes onto the task's empty stack the initial values
for its register. Then the task is added to the tasks queue.

Registers layout for the cortex-M4 processor:
    r0 function argument 1 / general purpose
    r1 function argument 2 / general purpose
    r2 function argument 3 / general purpose
    r3 function argument 4 / general purpose
    r4 --
    r5 |
    r6 |
    r7 |
    r8 | General purpose
    r9 |
    r10 |
    r11 |
    r12 --
    r13 stack pointer
    r14 link register
    r15 program counter

The initialization of a new task's stack is done by mimicking the
image of the stack when program execution is interruped by an 
interrupt handler.
*/

extern void kcreate_task(void (*code)(void *), void *args, uint8_t priority);

/*
This is the kernel space implementation of the exit() system call.
*/

void kexit() {
    // The TaskTCB of the currently running task is deallocated.
    free(RUNNING);

    // The pointer to the running task is set to NULL
    RUNNING = NULL;

    // Then the SysTick counter is reset.
    SysTick_reset();
    
    // Finally the scheduler is invoked.
    PendSVTrigger();
}

void unknownService(void) {
    for (;;) {
        uint32_t i;
        for (i = 0; i < 0xFFFFF; i++) {
            // busy waiting
        }
    }
}
