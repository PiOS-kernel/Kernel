#include "systick.h"

static SysTick* SYSTICK = (SysTick*)BASE;

void SysTick_init(){
    SYSTICK->CTRL = 0x00000004;
    SYSTICK->RELOAD = 0x00000000;
    SYSTICK->CURRENT = 0x00000000;

    // set 31st and 30th bit to ZERO (CLEAN)
    // set [23th-0th] bits to ONE (SET) -> VALUE IS NOT CORRECT, NEED TO INCREASE OVERALL CLOCK
    SYSTICK->CALIB &= ~(0xC0000000);
    SYSTICK->CALIB |= MASK24;
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