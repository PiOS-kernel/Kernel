#include "synch.h"
#include "../heap/malloc.h"
#include "../exceptions.h"


MCB* mutex_init() {
    MCB* mcb = (MCB*) alloc(sizeof(MCB));
    mcb->lock = 0;
    mcb->owners = 0; // null
    mcb->type = MUTEX;
    return mcb;
}

MCB* semaphore_init(uint32_t c){
    MCB* mcb = (MCB*) alloc(sizeof(MCB));
    mcb->lock = 0;
    mcb->count = c;
    if(c < 1){ return NULL; }
    if(c == 1){
        mcb->type = SEMAPHORE_BIN;
        mcb->owners = 0; // null
    } else{
        mcb->type = SEMAPHORE_INT;
        mcb->owners = (uint32_t) dynamicList_init(MAX_SIZE_WAITING);
    }
    return mcb;
}

uint8_t mutex_wait(MCB* lock){
    disable_interrupts();
    if(lock->lock == 0){
        lock->lock = 1;
        lock->owners = RUNNING;
        enable_interrupts();
        return 1;
    } else{
        enable_interrupts();
        return 0;
    }
}

uint8_t mutex_post(MCB* lock){
    disable_interrupts();
    if(lock->lock == 1 && lock->owners == RUNNING){
        lock->lock = 0;
        lock->owners = NULL;
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


/* -------------------------- */

/* Dynamic List Implementation */
/**
 * @brief inizialized a dynamic list of 32-bit items

 * @param size dimension of the list
 */
dynamicList_t* dynamicList_init(uint8_t size){
    dynamicList_t* list = (dynamicList_t*) alloc(sizeof(dynamicList_t) + sizeof(uint32_t) * size);
    list->base = (uint32_t) list + sizeof(dynamicList_t);
    list->size = size;
    uint32_t address;
    for(int i = 0; i < list->size; i++){
        address = list->base + i * sizeof(uint32_t);
            *(uint32_t*)address = 0;
    }
    return list;
}

/**
 * @brief add an item to the list. This function does NOT check overflows
 * 
 * @param list 
 * @param item 
 * @return uint8_t
 */
uint8_t dynamicList_add(dynamicList_t* list, uint32_t item){
    uint32_t address;
    for(int i = 0; i < list->size; i++){
        address = list->base + i * sizeof(uint32_t);
        if(*(uint32_t*)address == 0){
            *(uint32_t*)address = (uint32_t) item;
            return 1;
        }
    }
    // list is full
    return 0;
}

/**
 * @brief remove the first occurrence founded in the list
 * 
 * @param list 
 * @param item 
 * @return uint8_t 
 */
uint8_t dynamicList_remove(dynamicList_t* list, uint32_t item){
    uint32_t address;
    for(int i = 0; i < list->size; i++){
        address = list->base + i * sizeof(uint32_t);
        if(*(uint32_t*)address == item){
            *(uint32_t*)address = 0;
            return 1;
        }
    }
    // item not found
    return 0;
}

/**
 * @brief search a specific item
 * 
 * @param list 
 * @return uint32_t 
 */
uint32_t* dynamicList_search(dynamicList_t* list, uint32_t item){
    uint32_t address;
    for(int i = 0; i < list->size; i++){
        address = list->base + i * sizeof(uint32_t);
        if(*(uint32_t*)address == item){
            return (uint32_t*)address;
        }
    }
    // item not found
    return NULL;
}