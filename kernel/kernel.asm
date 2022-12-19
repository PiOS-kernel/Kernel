    .thumb

; -----------------------------------------------------------
SVCallISR: .asmfunc
    ; Get the SVC number
    ldr r4, [r7, #40]
    ldrb r4, [r4, #-2]

    ; Dispatch to the requested service
    cmp r4, #01
    itt eq
    ldreq r5, =kcreate_task
    beq _callService
    ldr r5, =unknownService

    ; Call the service
    _callService: str lr, [sp, #-4]!
    blx r5
    ldr pc, [sp], #4
    .endasmfunc