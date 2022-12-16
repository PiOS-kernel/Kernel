#include <stdint.h>

typedef void(fn_ptr)(void*);

/*
This enum lists all the services that can be requested by an application to
the kernel.
Each service has a numeric identifier.
*/
typedef enum
{
    CREATE_TASK_ID = 1,
} SysCallID;

/*
This is the system call provided to the user application, in order to
create a new task.
It accepts a function pointer, a pointer to its arguments, and a priority.
The function simply invokes the kernel to request the given service.
*/

void create_task(fn_ptr code, void* args, uint32_t priority) {
    asm volatile (
        "svc %[syscall_id]\n\t"
        "mov pc, lr\n"
        :
        : [syscall_id] "I" (CREATE_TASK_ID)
        :
    );
}

void unknownService(void) {
    for (;;) {
        for (uint32_t i = 0; i < 0xFFFFF; i++) {
            // busy waiting
        }
    }
}

/*

kcreate_task(), brief description:
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

    Task initialization:
    When a task is prehempted by the SysTick interrupt handler, its
    program counter is saved in the link register. Then the SysTick
    handler calls the routine that performs the context switch, which
    pushes r0 through r12, and r14 (link register) onto the stack.
    Therefore a new task's stack needs to be initialized by pushing
    the necessary values for registers r0-r12 and for the link register,
    which should hold the memory address of the first instruction to be
    executed by the task.
*/