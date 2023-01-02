#include "exceptions.h"
#include "kernel.h"
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

typedef struct SysTick {
    uint32_t CTRL __attribute__((aligned(4)));
    uint32_t RELOAD __attribute__((aligned(4)));
    uint32_t CURRENT __attribute__((aligned(4)));
    uint32_t CALIB __attribute__((aligned(4)));
} SysTick;

/* Number of time units (in milliseconds) dedicated to a single task, before interrupting it */

const uint32_t TASK_TIME_UNITS = 1;

static const uint32_t MASK24 = 0x00FFFFFF;

/* Memory mapped address for the SysTick configuration structure */
static const size_t BASE = 0xe000e010;

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

void SysTick_reset() {
    SYSTICK->CURRENT = 0x00000000;
    TICKS_COUNTER = 0;
}
