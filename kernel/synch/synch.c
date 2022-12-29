#include "synch.h"
#include "../heap/malloc.h"
#include "../exceptions.h"

MCB* mutex_init(void) {
    MCB* mcb = (MCB*) alloc(sizeof(MCB));
    mcb->lock = 0;
    mcb->owner = NULL;
    return mcb;
}

uint8_t MCB_wait(MCB* lock){
    disable_interrupts();
    if(lock->lock == 0){
        lock->lock = 1;
        lock->owner = RUNNING;
        enable_interrupts();
        return 1;
    } else{
        enable_interrupts();
        return 0;
    }
}

uint8_t MCB_post(MCB* lock){
    disable_interrupts();
    if(lock->lock == 1 && lock->owner == RUNNING){
        lock->lock = 0;
        lock->owner = NULL;
        enable_interrupts();
        return 1;
    }
    enable_interrupts();
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
