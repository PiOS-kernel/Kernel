#include "../../../task/task.h"
#include "../../../utils/utils.h"
#include "../../../heap/malloc.h"

/* 
On exit from an exception or interrupt handler, when the cortex m4 processor
is running in thumb mode it expects bit[0] of the next instruction to execute to
be set to 0.
*/

#define PC_MASK 0xFFFFFFFE

/* This is the initial value for the XPSR register of a newly created task. */
const uint32_t INITIAL_XPSR = 0x01000000;

/* 
When a task is created its stack is initialized to resemble what it would
look like if the task was interrupted by an interrupt handler. This is
the value for exception return.
WE WILL NEED TO CHANGE THIS VALUE ONCE WE USE 'MSP' and 'PSP'.
*/
#define INITIAL_EXEC_RETURN 0xFFFFFFFD

/* Port specific implementation of kcreate_task */

void kcreate_task(void (*code)(void *), void *args, uint8_t priority) {
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

    // Now the execution return value from the ISR is pushed onto the stack.
    tcb->stp -= 4;
    *((uint32_t*) tcb->stp) = INITIAL_EXEC_RETURN;

    // Finally, registers r4 through r11 are pushed onto the stack and
    // 0-initialized.
    stack_push(tcb, (uint8_t*) zeros, sizeof(size_t) * 8);

    // The task is inserted into the tasks queue
    enqueue(&READY_QUEUES[priority], tcb);
}