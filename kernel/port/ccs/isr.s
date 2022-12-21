	.thumb
	.ref kcreate_task
	.ref unknownService

tmpKcreate_task:		.word kcreate_task
tmpUnknownService:		.word unknownService
SVCallISR: .asmfunc
    ; Get the SVC number
    ldr r4, [r7, #40]
    ldrb r4, [r4, #-2] ; -2

    ; Dispatch to the requested service
    cmp r4, #0x1
    itt eq
    ldreq r5, tmpKcreate_task
    beq callService
    ldr r5, tmpUnknownService

    ; Call the service
callService:
    str lr, [sp, #-4]! ; -4
    blx r5
    ldr pc, [sp], #4

    .endasmfunc
