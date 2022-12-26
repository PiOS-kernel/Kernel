	.thumb
    .global SVCallISR
    .global task_switch
    .global create_task
    .global execute_first_task
    .global SysTick_Handler
    .global PendSV_Handler
	.ref kcreate_task
	.ref unknownService
	.ref RUNNING
	.ref TICKS_COUNTER
	.ref schedule
	.ref INITIAL_XPSR
	.ref IRQ_CTRL_REGISTER
	.ref PEND_SV_BIT

constKcreate_task:		    .word kcreate_task
constUnknownService:		.word unknownService
constSchedule:	            .word schedule
constRUNNING:	            .word RUNNING
constTICKS_COUNTER:	        .word TICKS_COUNTER
constINITIAL_XPSR: 			.word INITIAL_XPSR
constIRQ_CTRL_REGISTER:		.word IRQ_CTRL_REGISTER
constPEND_SV_BIT: 			.word PEND_SV_BIT

SVCallISR: .asmfunc
    ; Get the SVC number
    ldr r4, [sp, #24]
    ldrb r4, [r4, #-2] ; -2

    ; Dispatch to the requested service
    cmp r4, #0x1
    itt eq
    ldreq r5, constKcreate_task
    beq callService
    ldr r5, constUnknownService

    ; Call the service
callService:
    str lr, [sp, #-4]! ; -4
    blx r5
    ldr pc, [sp], #4

    .endasmfunc

; -------------
; This function does the context switch for a task.
; It stores the current values in the registers to the current task's stack,
; calls the schedule function, and loads the new task's stack in the registers.

task_switch: .asmfunc
    ; Interrupts are disabled
    cpsid i

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

    ; The task's coreregisters are saved onto the stack
    ; It is not necessary to save other registers because this function
    ; is always called from the PendSV ISR, which saves and restores r0-r3, 
    ; r12, r14, r15 and XPSR.
    stmdb r13!, {r4-r11}

    ; The stack pointer is saved in the task's struct
    str r13, [r0]

    ; SCHEDULING:
    ; the scheduling algorithm determines wich task should be executed
scheduling_section:
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
    msr basepri, r0
    isb

    mov pc, lr

    .endasmfunc

; -------------

; This function simply triggers the PendSV interrupt, in order to 
; perform a context switch to the task that has been created at system
; boot time.

execute_first_task: .asmfunc
    ; The PendSV handler is triggered
    ldr r0, constIRQ_CTRL_REGISTER
    ldr r1, constPEND_SV_BIT
    ldr r0, [r0]
    ldr r1, [r1]
    str r1, [r0]

    .endasmfunc

; -------------

; The PendSV ISR, which calls the scheduler to perform a context switch
PendSV_Handler: .asmfunc
    ; The task switch is performed
    push {lr}
    bl task_switch
    pop {pc}
    .endasmfunc

; -------------

; The SysTick ISR
SysTick_Handler: .asmfunc
	; The ticks counter is incremented
    ldr r0, constTICKS_COUNTER
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]

    ; If the ticks counter has reached the value of 10 (10ms is the time
    ; quantum), the task switch is performed
    cmp r1, #10
    bne end_of_isr

    ; The ticks counter is reset
    mov r1, #0
    str r1, [r0]

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
