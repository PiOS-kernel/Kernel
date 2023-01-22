#include "testing.h"
#include <stdint.h>

#define SRAM_START  0x20000000U
#define SRAM_SIZE   (64U * 1024U) //128KB
#define SRAM_END    ((SRAM_START) + (SRAM_SIZE))

#define STACK_START   SRAM_END

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _la_data;

extern uint32_t _sbss;
extern uint32_t _ebss;

//prototype of main

int main(void);

static void _defaultHandler(void);

void ResetISR();
__attribute__((weak, alias("_defaultHandler"))) void NmiISR();
__attribute__((weak, alias("_defaultHandler"))) void HardFaultISR();
__attribute__((weak, alias("_defaultHandler"))) void MemManageISR();
__attribute__((weak, alias("_defaultHandler"))) void BusFaultISR();
__attribute__((weak, alias("_defaultHandler"))) void UsageFaultIsr();
extern void SVC_Handler();
__attribute__((weak, alias("_defaultHandler"))) void DebugMonitorISR();
__attribute__((weak, alias("_defaultHandler"))) void PendsvISR();
__attribute__((weak, alias("_defaultHandler"))) void SysTickISR();
__attribute__((weak, alias("_defaultHandler"))) void IRQGeneralISR();


// ISR VECTOR
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))((uint32_t)STACK_START),
                                            // The initial stack pointer
    ResetISR,                               // The reset handler
    NmiISR,                                 // The NMI handler
    HardFaultISR,                           // The hard fault handler
    MemManageISR,                           // The MPU fault handler
    BusFaultISR,                            // The bus fault handler
    UsageFaultIsr,                          // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    SVC_Handler,						        // SVCall handler
    DebugMonitorISR,                        // Debug monitor handler
    0,                                      // Reserved
    PendsvISR,                              // The PendSV handler
    SysTickISR,                             // The SysTick handler
    IRQGeneralISR,                          // IRQ general
};



void _defaultHandler()
{
	while(1);
}

void ResetISR()
{
	//copy .data section to SRAM
	uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata;
	
	uint8_t *pDst = (uint8_t*)&_sdata; //sram
	uint8_t *pSrc = (uint8_t*)&_la_data; //flash
	
	for(uint32_t i =0 ; i < size ; i++)
	{
		*pDst++ = *pSrc++;
	}
	
	//Init. the .bss section to zero in SRAM
	size = (uint32_t)&_ebss - (uint32_t)&_sbss;
	pDst = (uint8_t*)&_sbss;
	for(uint32_t i =0 ; i < size ; i++)
	{
		*pDst++ = 0;
	}
	
    __asm__("MOV R0, #01");
    __asm__("MSR CONTROL, R0");
	main();

    while(1);
}
