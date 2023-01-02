#include "synch.h"
#include "../heap/malloc.h"
#include "../exceptions.h"

MCB* mutex_init() {
    MCB* mcb = (MCB*) alloc(sizeof(MCB));
    mcb->lock = 0;
    mcb->owner = NULL;
    mcb->type = MUTEX;
    return mcb;
}

MCB* semaphore_init(uint32_t c){
    MCB* mcb = (MCB*) alloc(sizeof(MCB));
    mcb->lock = 0;
    mcb->owner = NULL;
    if(c < 1){ return NULL; }
    if(c == 1){
        mcb->type = SEMAPHORE_BIN;
    } else{
        mcb->type = SEMAPHORE_INT;
    }
    mcb->count = c;
    return mcb;
}

uint8_t mutex_wait(MCB* lock){
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

uint8_t mutex_post(MCB* lock){
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

uint8_t sem_wait(MCB* lock){
    disable_interrupts();
    if(lock->lock < lock->count){
        lock->lock += 1;
        enable_interrupts();
        return 1;
    } else{
        enable_interrupts();
        return 0;
    }
}

uint8_t sem_post(MCB* lock){
    disable_interrupts();
    if(lock->lock > 0){
        lock->lock -= 1;
        enable_interrupts();
        return 1;
    }
    enable_interrupts();
    return 0;
}

void synch_wait(MCB* lock){
    if(lock->type == MUTEX){
        while(!mutex_wait(lock)){
            PendSVTrigger();
        };
    } else {
        while(!sem_wait(lock)){
            PendSVTrigger();
        };
    }
    // priority inheritance ?
    // call context switch
    
}

void synch_post(MCB* lock){
    if(lock->type == MUTEX){
        mutex_post(lock);
    } else {
        sem_post(lock);
    }
}
