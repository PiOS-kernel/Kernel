#include "syscalls.h"
#include "../task/task.h"
#include "stddef.h"
#include "stdint.h"
#include "../heap/malloc.h"
#include "../utils/utils.h"

/*
This enum lists all the services that can be requested by an application to
the kernel.
Each service has a numeric identifier.
*/

typedef enum {
    CREATE_TASK_ID = 1,
} SysCallID;

/*
This is the system call provided to the user application, in order to
create a new task.
It accepts a function pointer, a pointer to its arguments, and a priority.
The function simply invokes the kernel to request the given service.
*/

void create_task(void (*code)(void *), void* args, uint8_t priority) {
    asm volatile (
        "svc %[syscall_id]\n\t"
        "mov pc, lr\n"
        :
        : [syscall_id] "I" (CREATE_TASK_ID)
        :
    );
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

void kcreate_task(void (*code)(void *), void *args, uint8_t priority) {
    // The task's TCB is created
    TaskTCB tcb;
    TaskTCB_init(&tcb, priority);

    // The link register is pushed onto the stack, and initialized to be
    // the memory address of the first instruction executed by the task
    stack_push(&tcb, (uint8_t*) &code, sizeof(void *));

    // Registers r1 through r12 are pushed onto the stack and
    // 0-initialized.
    // 12 * 4 bytes are copied to the stack, where 4 bytes is the size of
    // one register.
    size_t zeros[12];
    memset((uint8_t*) zeros, 0, sizeof(size_t) * 12);
    // The memory address of the first item in the array is given as source
    stack_push(&tcb, (uint8_t*) &zeros[0], sizeof(size_t) * 12);

    // The pointer to the arguments is saved in register r0.
    // The ARM ABI specifies that the first 4 32-bit function arguments
    // should be put in registers r0-r3.
    stack_push(&tcb, (uint8_t*) &args, sizeof(void *));

    TaskTCB *heap_allocated_tcb = (TaskTCB*) malloc(sizeof(TaskTCB));
    *heap_allocated_tcb = tcb;
    heap_allocated_tcb->stp = stack_end(heap_allocated_tcb) - 14 * 4;

    // The task is inserted into the tasks queue
    enqueue(&WAITING_QUEUES[priority], heap_allocated_tcb);
}

void unknownService(void) {
    for (;;) {
        for (uint32_t i = 0; i < 0xFFFFF; i++) {
            // busy waiting
        }
    }
}