    .cpu cortex-m4
    .thumb
    .syntax unified

@ -----------------------------------------------------------

.thumb_func
.global SVCallISR
SVCallISR:
    @ Get the SVC number
    ldr r4, [sp, #24]
    ldrb r4, [r4, #-2] @ -2 

    @ Dispatch to the requested service
    cmp r4, #0x1
    itt eq
    ldreq r5, =kcreate_task
    beq _callService
    ldr r5, =unknownService

    @ Call the service
    _callService: str lr, [sp, #-4]! @ -4
    blx r5
    ldr pc, [sp], #4

@ -----------------------------------------------------------

@ This function does the context switch for a task.
@ It stores the current values in the registers to the current task's stack,
@ calls the schedule function, and loads the new task's registers.
.thumb_func
.global task_switch
task_switch:
    @ Interrupts are disabled
    cpsid i

    @ r0 is loaded with the pointer to the running task
    ldr r0, =RUNNING
    ldr r0, [r0]
    
    @ SAVE:
    @ Because the first 32 bits of the TaskTCB struct are dedicated to the stack
    @ pointer, the value of r13 will be saved at that memory location before
    @ context switching
    
    @ If there is currently no running task, skip the SAVE part and
    @ branch to the scheduler
    cmp r0, #0
    beq _scheduling_section

    @ IF WE WHERE USING 'MSP' AND 'PSP', HERE WE WOULD NEED TO LOAD THE
    @ TASK'S STACK POINTER INTO 'R13' BEFORE SAVING THE REGISTERS
    @ The task's registers are saved onto the stack
    stmdb r13!, {r4-r11}  
    @ The stack pointer is saved in the task's struct
    str r13, [r0] 

    @ SCHEDULING:
    @ the scheduling algorithm determines wich task should be executed
_scheduling_section:
    push {lr}
    bl schedule
    pop {lr}

    @ RESUME:
    @ according to the ARM ABI convention the return value of 'schedule()',
    @ which is the pointer to the new running task, is saved in register r0

    @ the first struct field is the SP
    ldr r13, [r0]
    @ the task's registers are popped from the stack
    ldmia r13!, {r4-r11}

    @ Interrupts are enabled again
    cpsie i
    @ The register that tracks the current privilege level of the CPU
    @ is modified to return to user mode
    mov r0, #1
    msr basepri, r0
    isb
    @ At the top of the stack there is the return address to the task code
    mov pc, lr


@ -----------------------------------------------------------

@ The following is the system call that allows the creation of a new task
.thumb_func
.global create_task
create_task:
    svc #1
    mov pc, lr