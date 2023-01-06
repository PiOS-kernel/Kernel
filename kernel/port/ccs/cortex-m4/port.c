#ifdef YIELD_FROM_ISR
#undef YIELD_FROM_ISR
#endif
#define YIELD_FROM_ISR() __asm("cpsid i");    \
    __asm("ldr r0, constIRQ_CTRL_REGISTER "); \ 
    __asm("ldr r1, constPEND_SV_BIT");        \
    __asm("ldr r0, [r0]");                    \
    __asm("ldr r1, [r1]");                    \
    __asm("str r1, [r0]");                    \
    __asm("cpsie i")