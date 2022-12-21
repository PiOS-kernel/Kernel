#ifndef __PORT_H__
#define __PORT_H__

#include <stdint.h>
#include <stddef.h>

/* BIT DECLARATION -> TO BE MOVED IN TOOLS.C*/
#define BIT0                                     (uint16_t)(0x0001)
#define BIT1                                     (uint16_t)(0x0002)
#define BIT2                                     (uint16_t)(0x0004)
#define BIT3                                     (uint16_t)(0x0008)
#define BIT4                                     (uint16_t)(0x0010)
#define BIT5                                     (uint16_t)(0x0020)
#define BIT6                                     (uint16_t)(0x0040)
#define BIT7                                     (uint16_t)(0x0080)
#define BIT8                                     (uint16_t)(0x0100)
#define BIT9                                     (uint16_t)(0x0200)
#define BITA                                     (uint16_t)(0x0400)
#define BITB                                     (uint16_t)(0x0800)
#define BITC                                     (uint16_t)(0x1000)
#define BITD                                     (uint16_t)(0x2000)
#define BITE                                     (uint16_t)(0x4000)
#define BITF                                     (uint16_t)(0x8000)
#define BIT(x)                                 ((uint16_t)1 << (x))
/*------------*/

static const uint32_t MASK24 = 0x00FFFFFF;


static const size_t BASE = 0xe000e010;

typedef struct SysTick {

    uint32_t CTRL __attribute__((aligned(4)));
    uint32_t RELOAD __attribute__((aligned(4)));
    uint32_t CURRENT __attribute__((aligned(4)));
    uint32_t CALIB __attribute__((aligned(4)));



    /* wrong implementation
    volatile struct Ctrl {
        
        struct Bits {
            uint32_t ENABLE : 1;  // Counter enable
            uint32_t TICKINT : 1;  // SysTick exception request enable
            uint32_t CLKSOURCE : 1;  // Clock source selection
            uint32_t : 13;
            uint32_t COUNTFLAG : 1;  // Returns 1 if timer counted to 0 since last time this was read.
            uint32_t : 15;
        } b;

    } CTRL; // SysTick control and status register

    volatile struct Load {

    } LOAD; // SysTick reload value register

    volatile struct Val {

    } VAL; // SysTick reload value register

    volatile struct Calib {

    } CALIB; // SysTick calibration value register
    */

}SysTick;

/**
 * @brief SysTick module initialization
 *      set CTRL to default values
 *      clean all other register
 *      set CALIB according to specs:
 *          set 31st and 30th bit to ZERO (CLEAN),
 *          set [23th-0th] bits to ONE (SET) -> VALUE IS NOT CORRECT, NEED TO INCREASE OVERALL CLOCK
 */
void SysTick_init();
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

#endif