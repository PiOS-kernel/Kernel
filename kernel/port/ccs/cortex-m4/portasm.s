    .thumb
    .global SVC_Handler
    .global PendSV_Handler
    .global start_scheduler
    .global SysTick_Handler
    .global create_task
    .global exit
    .global yield
    .global kill
    .global enable_interrupts
    .global disable_interrupts
    .global PendSVTrigger
    .ref kcreate_task
    .ref kexit
    .ref kyield
    .ref kkill
    .ref unknownService
    .ref RUNNING
    .ref schedule
    .ref IRQ_CTRL_REGISTER
    .ref PEND_SV_BIT
    .ref TICKS_COUNTER
    .ref TASK_TIME_UNITS
    .ref CLOCK
    .ref pre_context_switch

constkcreate_task:        .word kcreate_task
constkexit:        .word kexit
constkyield:        .word kyield
constkkill:        .word kkill
constunknownService:        .word unknownService
constRUNNING:        .word RUNNING
constIRQ_CTRL_REGISTER:        .word IRQ_CTRL_REGISTER
constPEND_SV_BIT:        .word PEND_SV_BIT
constTICKS_COUNTER:        .word TICKS_COUNTER
constTASK_TIME_UNITS:        .word TASK_TIME_UNITS
constCLOCK:        .word CLOCK
constpre_context_switch:        .word pre_context_switch
; ----------------------------------------------------------- 
SVC_Handler: .asmfunc

    ; Get the SVC number 
    ldr r4, [sp, #24] 
    ldrb r4, [r4, #-2] ; -2 

    ; Dispatch to the requested service 
    cmp r4, #0x1 
    itt eq 
    ldreq r5, constkcreate_task 
    beq _callService 
    cmp r4, #0x2 
    itt eq 
    ldreq r5, constkexit 
    beq _callService 
    cmp r4, #0x3 
    itt eq 
    ldreq r5, constkyield 
    beq _callService 
    cmp r4, #0x4 
    itt eq 
    ldreq r5, constkkill 
    beq _callService 

    ; No service corresponding to the SVC number is found 
    ldr r5, constunknownService 
_callService: 

    ; Call the service 
    str lr, [sp, #-4]! ; -4 
    blx r5 
    ldr pc, [sp], #4 

    .endasmfunc


; ------------- 
; The PendSV ISR, which calls the scheduler to perform a context switch 
PendSV_Handler: .asmfunc

    ; Interrupts are disabled 
    cpsid i 
    isb 

    ; r0 is loaded with the pointer to the running task 
    ldr r0, constRUNNING 
    ldr r0, [r0] 

    ; SAVE: 
    ; Because the first 32 bits of the TaskTCB struct are dedicated to the stack 
    ; pointer, the value of r13 will be saved at that memory location before 
    ; context switching 
    ; If there is currently no running task, skip the SAVE part and 
    ; branch to the scheduler 
    cmp r0, #0 
    beq scheduling_section 

    ; IF WE WHERE USING 'MSP' AND 'PSP', HERE WE WOULD NEED TO LOAD THE 
    ; TASK'S STACK POINTER INTO 'R13' BEFORE SAVING THE REGISTERS 
    ; The task's core registers are saved onto the stack 
    ; It is not necessary to save other registers because this function 
    ; is always called from the PendSV ISR, which saves and restores r0-r3, 
    ; r12, r14, r15 and XPSR. 
    stmdb r13!, {r4-r11} 

    ; The stack pointer is saved in the task's struct 
    str r13, [r0] 
scheduling_section: 

    ; SCHEDULING: 
    ; the scheduling algorithm determines wich task should be executed 
    push {lr} 
    bl schedule 
    pop {lr} 

    ; RESUME: 
    ; according to the ARM ABI convention the return value of 'schedule()', 
    ; which is the pointer to the new running task, is saved in register r0 
    ; the first struct field is the SP 
    ldr r13, [r0] 

    ; the task's core registers are popped from the stack 
    ldmia r13!, {r4-r11} 

    ; Interrupts are enabled again 
    cpsie i 

    ; The register that tracks the current privilege level of the CPU 
    ; is modified to return to user mode 
    mov r0, #1 
    msr control, r0 

    ; ----------------- 
    isb 
    bx lr 

    .endasmfunc


; ------------- 
; This function simply triggers the PendSV interrupt, in order to 
; perform a context switch to the task that has been created at system 
; boot time. 
start_scheduler: .asmfunc

    ; The PendSV handler is triggered 
    ldr r0, constIRQ_CTRL_REGISTER 
    ldr r1, constPEND_SV_BIT 
    ldr r0, [r0] 
    ldr r1, [r1] 
    str r1, [r0] 

    .endasmfunc


; ------------- 
; The SysTick ISR 
SysTick_Handler: .asmfunc

    ; The ticks counter is incremented 
    ldr r0, constTICKS_COUNTER 
    ldr r1, [r0] 
    add r1, r1, #1 
    str r1, [r0] 

    ; If the ticks counter has reached the value of TASK_TIME_UNITS (10ms is the default time 
    ; quantum), the task switch is performed 
    ldr r2, constTASK_TIME_UNITS 
    ldr r2, [r2] 
    cmp r1, r2 
    bne end_of_isr 

    ; The ticks counter is reset 
    mov r1, #0 
    str r1, [r0] 

    ; The clock counter is incremented 
    ldr r0, constCLOCK 
    ldr r1, [r0] 
    add r1, r1, #1 
    str r1, [r0] 

    ; procedure called before context switch (user-defined) 
    ldr r5, constpre_context_switch 
    str lr, [sp, #-4]! ; -4 
    blx r5 
    ldr lr, [sp], #4 

    ; The PendSV handler is triggered 
    ldr r0, constIRQ_CTRL_REGISTER 
    ldr r1, constPEND_SV_BIT 
    ldr r0, [r0] 
    ldr r1, [r1] 
    str r1, [r0] 
end_of_isr: 
    bx lr 

    .endasmfunc


; ------------- 
; Syscall to create a new task 
create_task: .asmfunc
    svc #1 
    mov pc, lr 

    .endasmfunc


; ----------------------------------------------------------- 
; The system call that allows a task to terminate itself 
exit: .asmfunc
    svc #2 
    mov pc, lr 

    .endasmfunc


; ----------------------------------------------------------- 
; The system call that allows a task to yield the cpu 
yield: .asmfunc
    svc #3 
    mov pc, lr 

    .endasmfunc


; ----------------------------------------------------------- 
; System call to kill a running task 
kill: .asmfunc
    svc #4 
    mov pc, lr 

    .endasmfunc


; ----------------------------------------------------------- 
; The following are utilities for activating and deactivating interrupts 
enable_interrupts: .asmfunc
    cpsie i 
    bx lr 

    .endasmfunc

disable_interrupts: .asmfunc
    cpsid i 
    bx lr 

    .endasmfunc

PendSVTrigger: .asmfunc

    ; The PendSV handler is triggered 
    cpsid i 
    ldr r0, constIRQ_CTRL_REGISTER 
    ldr r1, constPEND_SV_BIT 
    ldr r0, [r0] 
    ldr r1, [r1] 
    str r1, [r0] 
    cpsie i 
    bx lr 

    .endasmfunc

