#include "syscalls.h"
#include "../task/task.h"
#include "../heap/malloc.h"
#include "../utils/utils.h"
#include "../exceptions.h"
#include <stddef.h>
#include <stdint.h>

/* 
On exit from an exception or interrupt handler, when the cortex m4 processor
is running in thumb mode it expects bit[0] of the next instruction to execute to
be set to 0.
*/

#define PC_MASK 0xFFFFFFFE

/* This is the initial value for the XPSR register of a newly created task. */
const uint32_t INITIAL_XPSR = 0x01000000;

/**
 * @brief: This is the system call provided to the user application, in order to
 * create a new task.
 * The function simply invokes the kernel to request the given service.
 * @param: pointer to the task entry point.
 * @param: pointer to the task arguments.
 * @param: priority.
 * @return: an handle to the created task, which serves as identifier.
*/

extern TaskHandle create_task(void (*code)(void *), void* args, uint8_t priority);

/*
This is the system call provided to the user application to terminate execution
of the current task.
*/

extern void task_exit();

/*
This is the system call provided to the user application to make the current running task 
yield the cpu and reset the sceduling alghoritm
*/

extern void yield();

/**
 *@brief: System call to terminate the execution of a task.
 *@param: handle of the task that should be killed.
*/

extern void kill(TaskHandle task);

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

TaskHandle kcreate_task(void (*code)(void *), void *args, uint8_t priority) {
    // The task's TCB is created
    TaskTCB* tcb = (TaskTCB*) alloc(sizeof(TaskTCB));
    TaskTCB_init(tcb, priority);

    uint32_t zeros[12];
    memset((uint8_t*) zeros, 0, sizeof(uint32_t) * 12);

    // The XPSR register is pushed onto the stack.
    tcb->stp -= 4;
    *((uint32_t*) tcb->stp) = INITIAL_XPSR;

    // The program counter is pushed onto the stack. It will contain a
    // pointer to the task's entry point.
    tcb->stp -= 4;
    *((uint32_t*) tcb->stp) = (uint32_t) code & PC_MASK;

    // Then the link register is pushed and initialized to 0.
    stack_push(tcb, (uint8_t*) zeros, sizeof(uint32_t));

    // Then r12, r3, r2, r1, and r0 are pushed onto the stack. Only r0
    // has a non-zero value, which is the pointer to the arguments.
    stack_push(tcb, (uint8_t*) zeros, sizeof(uint32_t) * 4);
    stack_push(tcb, (uint8_t*) &args, sizeof(void *));

    // Finally, registers r4 through r11 are pushed onto the stack and
    // 0-initialized.
    stack_push(tcb, (uint8_t*) zeros, sizeof(size_t) * 8);

    // The task is inserted into the tasks queue
    enqueue(&READY_QUEUES[priority], tcb);

    // The pointer to the TCB is placed in r0, where it will be found
    // by create_task.
    return (TaskHandle) tcb;
}

/*
Initialization routine for the IDLE task. This task will be executed when no other
task is in the READY state. Its stack is just big enough to store the task's registers.
*/

void idle_task_init() {
    IDLE_TASK = (TaskTCB*) alloc(sizeof(IdleTaskTCB));
    // the idle task runs at the lowest priority
    TaskTCB_init(IDLE_TASK, MIN_PRIORITY - 1);

    uint32_t zeros[12];
    memset((uint8_t*) zeros, 0, sizeof(uint32_t) * 12);

    // The XPSR register is pushed onto the stack.
    IDLE_TASK->stp -= 4;
    *((uint32_t*) IDLE_TASK->stp) = INITIAL_XPSR;

    // The program counter is pushed onto the stack. It will contain a
    // pointer to the task's entry point.
    IDLE_TASK->stp -= 4;
    *((uint32_t*) IDLE_TASK->stp) = (uint32_t) idle_task_code & PC_MASK;

    // Then the link register is pushed and initialized to 0.
    stack_push(IDLE_TASK, (uint8_t*) zeros, sizeof(uint32_t));

    // Then r12, r3, r2, r1, and r0 are pushed onto the stack. Only r0
    // has a non-zero value, which is the pointer to the arguments.
    stack_push(IDLE_TASK, (uint8_t*) zeros, sizeof(uint32_t) * 4);
    stack_push(IDLE_TASK, (uint8_t*) 0, sizeof(void *));

    // Finally, registers r4 through r11 are pushed onto the stack and
    // 0-initialized.
    stack_push(IDLE_TASK, (uint8_t*) zeros, sizeof(size_t) * 8);
}

/*
This is the kernel space implementation of the task_exit() system call.
*/

void ktask_exit() {
    // The TaskTCB of the currently running task is deallocated.
    mem_free((uint8_t*) RUNNING, sizeof(TaskTCB));

    // The pointer to the running task is set to NULL
    RUNNING = NULL;

    // Then the SysTick counter is reset.
    SysTick_reset();
    
    // Finally the scheduler is invoked.
    PendSVTrigger();
}

/*
This is the kernel implementation of the yield function
*/

void kyield() {
    // SysTick counter is reset.
    SysTick_reset();
    
    // The scheduler is invoked.
    PendSVTrigger();
}

/*
Kernel space implementation of the kill syscall. The given task is
removed from any task queue it is currently part of, and its memory
is deallocated.
*/

void kkill(TaskHandle task) {
    // The task is removed from any queue it is part of.
    unlink_task((TaskTCB*) task);

    // The task's memory is deallocated.
    mem_free((uint8_t*) task, sizeof(TaskTCB));
}

/* function called before the context switch */
void pre_context_switch(){
    return;
}

void unknownService(void) {
    for (;;) {
        uint32_t i;
        for (i = 0; i < 0xFFFFF; i++) {
            // busy waiting
        }
    }
}