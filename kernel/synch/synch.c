#include "synch.h"
#include "../heap/malloc.h"

/* Memory mapped address for the NIC control register */
const uint32_t IRQ_CTRL_REGISTER = 0xE000ED04;

/* Bit representing a pending 'PendSV' exception */
const uint32_t PEND_SV_BIT = 0x10000000;

extern void activate_interrupts(void);
extern void deactivate_interrupts(void);
extern void PendSVTrigger(void);

MCB* MCB_init(void) {
    MCB* mcb = (MCB*) alloc(sizeof(MCB));
    mcb->lock = 0;
    mcb->owner = NULL;
    return mcb;
}

uint8_t MCB_wait(MCB* lock){
    deactivate_interrupts();
    if(lock->lock == 0){
        lock->lock = 1;
        lock->owner = RUNNING;
        activate_interrupts();
        return 1;
    } else{
        activate_interrupts();
        return 0;
    }
}

uint8_t MCB_post(MCB* lock){
    deactivate_interrupts();
    if(lock->lock == 1 && lock->owner == RUNNING){
        lock->lock = 0;
        lock->owner = NULL;
        activate_interrupts();
        return 1;
    }
    activate_interrupts();
    return 0;
}

void mutex_wait(MCB* lock){
    while(!MCB_wait(lock)){
        // priority inheritance ?
        // call context switch
        PendSVTrigger();
    };
}

void mutex_post(MCB* lock){
    MCB_post(lock);
}
