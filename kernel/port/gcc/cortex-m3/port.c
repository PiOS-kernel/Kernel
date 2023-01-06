#ifdef YIELD_FROM_ISR()
#undef YIELD_FROM_ISR()
#endif
#define YIELD_FROM_ISR() asm("cpsid i" \
    asm("ldr r0, =IRQ_CTRL_REGISTER"); \ 
    asm("ldr r1, =PEND_SV_BIT"); \
    asm("ldr r0, [r0]"); \
    asm("ldr r1, [r1]"); \
    asm("str r1, [r0]"); \
    asm("cpsie i")