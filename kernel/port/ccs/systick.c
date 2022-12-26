#include "systick.h"
#include "kernel.h"
#include <stdint.h>
#define TASK_TIME_UNITS 10

/* Memory mapped address for the NIC control register */
const uint32_t IRQ_CTRL_REGISTER = 0xE000ED04;

/* Bit representing a pending 'PendSV' exception */
const uint32_t PEND_SV_BIT = 0x10000000;

/* SysTick configuration structure */
static SysTick* SYSTICK = (SysTick*)BASE;

/* Counter of the elapsed SysTick time units */
int TICKS_COUNTER = 0;

/* Code to cofigure and initialize SysTick */

void SysTick_init(int val){
    SYSTICK->CTRL = 0x00000004;
    SYSTICK->RELOAD = 0x00000000;
    SYSTICK->CURRENT = 0x00000000;

    // set 31st and 30th bit to ZERO (CLEAN)
    // set [23th-0th] bits to ONE (SET) -> VALUE IS NOT CORRECT, NEED TO INCREASE OVERALL CLOCK
    SYSTICK->CALIB &= ~(0xC0000000);
    SYSTICK->CALIB |= MASK24;

    SysTick_setLOAD(val);
}

void SysTick_enable(){
    SYSTICK->CTRL = 0x00000007;
}

void SysTick_disable(){
    SYSTICK->CTRL = 0x00000004;
}

void SysTick_setLOAD(int x){
    uint32_t value = x & MASK24;
    SYSTICK->RELOAD = value;
}
