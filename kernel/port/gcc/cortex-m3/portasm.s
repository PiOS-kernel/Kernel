.cpu cortex-m3
.thumb
.syntax unified

@ -----------------------------------------------------------

.thumb_func
.global SVC_Handler
SVC_Handler:
    @ Get the SVC number 
    ldr r4, [sp, #24] 
    ldrb r4, [r4, #-2] @ -2 
    
    @ Dispatch to the requested service 
    cmp r4, #0x1 
    itt eq 
    ldreq r5, =kcreate_task 
    beq _callService 
    
    cmp r4, #0x2 
    itt eq 
    ldreq r5, =ktask_exit 
    beq _callService 

    cmp r4, #0x3
    itt eq
    ldreq r5, =kyield
    beq _callService

    cmp r4, #0x4
    itt eq
    ldreq r5, =kkill
    beq _callService
    
    @ No service corresponding to the SVC number is found 
    ldr r5, =unknownService 
    
    @ Call the service 
_callService: 
    str lr, [sp, #-4]! @ -4 
    blx r5 
    ldr pc, [sp], #4 
    

@ ------------- 
@ The PendSV ISR, which calls the scheduler to perform a context switch 

.thumb_func
.global PendSV_Handler
PendSV_Handler:
    @ Interrupts are disabled 
    cpsid i 
    isb

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
    beq scheduling_section 
    
    @ IF WE WHERE USING 'MSP' AND 'PSP', HERE WE WOULD NEED TO LOAD THE 
    @ TASK'S STACK POINTER INTO 'R13' BEFORE SAVING THE REGISTERS 
    
    @ The task's core registers are saved onto the stack 
    @ It is not necessary to save other registers because this function 
    @ is always called from the PendSV ISR, which saves and restores r0-r3, 
    @ r12, r14, r15 and XPSR. 
    stmdb r13!, {r4-r11} 
    
    @ The stack pointer is saved in the task's struct 
    str r13, [r0] 
    
    @ SCHEDULING: 
    @ the scheduling algorithm determines wich task should be executed 
scheduling_section: 
    push {lr} 
    bl schedule 
    pop {lr} 
    
    @ RESUME: 
    @ according to the ARM ABI convention the return value of 'schedule()', 
    @ which is the pointer to the new running task, is saved in register r0 
    
    @ the first struct field is the SP 
    ldr r13, [r0] 
    @ the task's core registers are popped from the stack 
    ldmia r13!, {r4-r11} 
    
    @ Interrupts are enabled again 
    cpsie i 
    @ The register that tracks the current privilege level of the CPU 
    @ is modified to return to user mode 

    mov r0, #1 
    msr control, r0

    @ -----------------
    isb 
    
    bx lr 
    

@ ------------- 
@ This function simply triggers the PendSV interrupt, in order to 
@ perform a context switch to the task that has been created at system 
@ boot time. 
.thumb_func
.global start_scheduler
start_scheduler:
    @ The PendSV handler is triggered 
    ldr r0, =IRQ_CTRL_REGISTER 
    ldr r1, =PEND_SV_BIT 
    ldr r0, [r0] 
    ldr r1, [r1] 
    str r1, [r0] 
    

@ ------------- 
@ The SysTick ISR 
.thumb_func
.global SysTick_Handler
SysTick_Handler:

    @ The ticks counter is incremented 
    ldr r0, =TICKS_COUNTER 
    ldr r1, [r0] 
    add r1, r1, #1 
    str r1, [r0] 
    
    @ If the ticks counter has reached the value of TASK_TIME_UNITS (10ms is the default time 
    @ quantum), the task switch is performed 
    ldr r2, =TASK_TIME_UNITS
    ldr r2, [r2]
    cmp r1, r2
    bne end_of_isr 
    
    @ The ticks counter is reset 
    mov r1, #0 
    str r1, [r0]

    @ The clock counter is incremented
    ldr r0, =CLOCK 
    ldr r1, [r0] 
    add r1, r1, #1 
    str r1, [r0] 

    @ procedure called before context switch (user-defined)
    ldr r5, =pre_context_switch
    str lr, [sp, #-4]! @ -4 
    blx r5
    ldr lr, [sp], #4
    
    @ The PendSV handler is triggered 
    ldr r0, =IRQ_CTRL_REGISTER 
    ldr r1, =PEND_SV_BIT 
    ldr r0, [r0] 
    ldr r1, [r1] 
    str r1, [r0] 
    
end_of_isr: 
    bx lr 
    

@ ------------- 
@ Syscall to create a new task 
.thumb_func
.global create_task
create_task:
    svc #1 
    mov pc, lr 
    

@ ----------------------------------------------------------- 
@ The system call that allows a task to terminate itself 
.thumb_func
.global task_exit
task_exit:
    svc #2 
    mov pc, lr 


@ -----------------------------------------------------------

@ The system call that allows a task to yield the cpu 
.thumb_func
.global yield
yield:
    svc #3
    mov pc, lr


@ -----------------------------------------------------------

@ System call to kill a running task
.thumb_func
.global kill
kill:
    svc #4
    mov pc, lr
    

@ ----------------------------------------------------------- 
@ The following are utilities for activating and deactivating interrupts 
.thumb_func
.global enable_interrupts
enable_interrupts:
    cpsie i 
    bx lr
    

.thumb_func
.global disable_interrupts
disable_interrupts:
    cpsid i 
    bx lr 


.thumb_func
.global PendSVTrigger
PendSVTrigger:
    
    @ The PendSV handler is triggered 
    cpsid i
    ldr r0, =IRQ_CTRL_REGISTER 
    ldr r1, =PEND_SV_BIT 
    ldr r0, [r0] 
    ldr r1, [r1] 
    str r1, [r0]
    cpsie i
    bx lr 
    
