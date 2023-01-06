#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

// This macro allows to trigger the PendSV handler from an ISR
// whitout invoking the SVC handler.
#define YIELD_FROM_ISR()

/**
 * @brief SysTick module initialization
 *      set CTRL to default values
 *      clean all other register
 *      set CALIB according to specs:
 *          set 31st and 30th bit to ZERO (CLEAN),
 *          set [23th-0th] bits to ONE (SET) -> VALUE IS NOT CORRECT, NEED TO INCREASE OVERALL CLOCK
 */
void SysTick_init(int val);
void SysTick_enable();
void SysTick_disable();

/**
 * @brief to update LOAD register value
 * [23-0] bits are accessible; other are reserved
 * 
 * @param x number of ticks before triggering interrupt
 *      qemu -> 12MHz clock
 */
void SysTick_setLOAD(int x);

/*
Resets the SysTick counter to 0
*/
void SysTick_reset();

/*
Function to trigger the PendSV handler
*/
extern void PendSVTrigger();

#endif
