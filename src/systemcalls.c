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

void kcreate_task(void (*code)(void *), void *args, size_t priority) {
    // The task's TCB is created
    TaskTCB tcb = TaskTCB_new(NULL, priority);

    // The link register is pushed onto the stack, and initialized to be
    // the memory address of the first instruction executed by the task
    TaskTCB_stack_push(&tcb, &code, sizeof(void *));

    // Registers r1 through r12 are pushed onto the stack and
    // 0-initialized.
    // 12 * 4 bytes are copied to the stack, where 4 bytes is the size of
    // one register.
    size_t zeros[12] = {0};
    // The memory address of the first item in the array is given as source
    TaskTCB_stack_push(&tcb, &zeros[0], sizeof(size_t) * 12);

    // The pointer to the arguments is saved in register r0.
    // The ARM ABI specifies that the first 4 32-bit function arguments
    // should be put in registers r0-r3.
    TaskTCB_stack_push(&tcb, &args, sizeof(void *));

    TaskTCB *heap_allocated_tcb = malloc(sizeof(TaskTCB));
    *heap_allocated_tcb = tcb;
    heap_allocated_tcb->stp = heap_allocated_tcb->stack_end() - 14 * 4;

    // The task is inserted into the tasks queue
    WAITING_QUEUE_enqueue(heap_allocated_tcb);
}


// This function does the context switch for a task.
// It stores the current values in the registers to the current task's stack,
// calls the schedule function, and loads the new task's stack in the registers.
void task_switch(void) {
    // Interrupts are disabled
    __asm__ __volatile__("CPSID i");
    // The 'Rust' part of this function is called, to get the pointer to
    // the running task
    __asm__ __volatile__("STMDB r13!, {r14}");
    task_switch_prologue();
    __asm__ __volatile__("LDMIA r13!, {r14}");

    /*
    SAVE:
    At this point, r0 holds the pointer to the running task. Because
    the first 32 bits of the TaskTCB struct are dedicated to the stack
    pointer, the value of r13 will be saved at that memory location before
    context switching
    */
    // If there is currently no running task, skip the SAVE part and
    // branch to the scheduler
    __asm__ __volatile__("CMP r0, #0");
    __asm__ __volatile__("BEQ 2f");
    // The task's registers are saved onto the stack
    __asm__ __volatile__("STMDB r13!, {r4-r12, r14}");   
    // the stack pointer is loaded in r13 (sp register)               
    __asm__ __volatile__("STR r13, [r0]");

    /*
    SCHEDULING:
    the scheduling algorithm determines wich task should be executed
    */
    __asm__ __volatile__("2:");
    __asm__ __volatile__("STMDB r13!, {r14}");
    schedule();
    __asm__ __volatile__("LDMIA r13!, {r14}");

    /*
    RESUME:
    according to the ARM ABI convention the return value of 'schedule()',
    which is the pointer to the new running task, is saved in register r0
    */
    // the first struct field is the SP
    __asm__ __volatile__("LDR r13, [r0, #0]");
    // the task's registers are popped from the stack
    __asm__ __volatile__("LDMIA r13!, {r0-r12}");

    // Interrupts are enabled again
    __asm__ __volatile__("CPSIE i");
    // The register that tracks the current privilege level of the CPU
    // is modified to return to user mode
    __asm__ __volatile__("STMDB r13!, {r0}");
    __asm__ __volatile__("MOV r0, #1");
    __asm__ __volatile__("MSR basepri, r0");
    __asm__ __volatile__("ISB");
    __asm__ __volatile__("LDMIA r13!, {r0}");
    // At the top of the stack there is the return address to the task code
    __asm__ __volatile__("MOV pc, lr");
}