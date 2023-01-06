#include "synch.h"
#include "../heap/malloc.h"
#include "../exceptions.h"
#include "../task/task.h"


MCB* mutex_init() {
    MCB* mcb = (MCB*) alloc(sizeof(MCB));
    mcb->lock = 0;
    mcb->owners = dynamicList_init(1);
    mcb->type = MUTEX;
    int i = 0;
    for (; i<MIN_PRIORITY; i++) {
         Queue_init(&(mcb->waiting[i]));
    }
    mcb->max_priority_owners = -1;
    mcb->max_priority_waiting = -1;
    return mcb;
}

MCB* semaphore_init(uint32_t c){
    MCB* mcb = (MCB*) alloc(sizeof(MCB));
    mcb->lock = 0;
    mcb->count = c;
    if(c < 1){ return NULL; }
    if(c == 1){
        mcb->type = SEMAPHORE_BIN;
    } else{
        mcb->type = SEMAPHORE_INT;
    }
    mcb->owners = dynamicList_init(c);
    int i = 0;
    for (; i<MIN_PRIORITY; i++) {
         Queue_init(&(mcb->waiting[i]));
    }
    mcb->max_priority_owners = -1;
    mcb->max_priority_waiting = -1;
    return mcb;
}

uint8_t mutex_wait(MCB* lock){
    if(lock->lock == 0){
        lock->lock = 1;
        dynamicList_add(lock->owners, (uint32_t) RUNNING);
        if(lock->max_priority_owners > RUNNING->priority){
            lock->max_priority_owners = RUNNING->priority;
        }
        enable_interrupts();
        return 1;
    } else{
        return 0;
    }
}

uint8_t mutex_post(MCB* lock){
    if(lock->lock == 1 && lock->owners->base == (uint32_t) RUNNING){ // mutex has just 1 element
        lock->lock = 0;
        dynamicList_remove(lock->owners, (uint32_t) RUNNING);
        enable_interrupts();
        return 1;
    }
    return 0;
}

uint8_t sem_wait(MCB* lock){
    if(lock->lock < lock->count){
        dynamicList_add(lock->owners, (uint32_t) RUNNING);
        if(lock->max_priority_owners > RUNNING->priority){
            lock->max_priority_owners = RUNNING->priority;
        }
        lock->lock += 1;
        return 1;
    } else{
        return 0;
    }
}

uint8_t sem_post(MCB* lock){
    if(lock->lock > 0){
        lock->lock -= 1;
        dynamicList_remove(lock->owners, (uint32_t) RUNNING);
        enable_interrupts();
        return 1;
    }
    return 0;
}

void synch_wait(MCB* lock){
    disable_interrupts();
    uint8_t result;
    if(lock->type == MUTEX){
        result = mutex_wait(lock);
    } else {
        result = sem_wait(lock);
    }
    if(!result){
        // task failed to acquire the lock
        enqueue(&(lock->waiting[RUNNING->priority]), RUNNING);
        SHOULD_WAIT = 1;
        update_max_priority(lock->waiting, &(lock->max_priority_waiting));
        priority_inheritance(lock);
        enable_interrupts();
        PendSVTrigger();
        // ownership will be yielded by another task, it should NOT be self-assigned
    }
    enable_interrupts();
}

void synch_post(MCB* lock){
    disable_interrupts();
    if(lock->type == MUTEX){
        mutex_post(lock);
    } else {
        sem_post(lock);
    }
     // reset priority inheritance
    RUNNING->priority = RUNNING->default_priority;
    if(lock->max_priority_waiting != -1){ // waiting is not empty
        // choose the task to add to owners
        TaskTCB* task = dequeue(&(lock->waiting[lock->max_priority_waiting]));
        update_max_priority(lock->waiting, &(lock->max_priority_waiting));
        dynamicList_add(lock->owners, (uint32_t) task);
        lock->count += 1;
    }
    enable_interrupts();
}


/* -------------------------- */

/* Dynamic List Implementation & utilities */
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

/**
 * @brief update the max priority value of the vector of priority queues
 * 
 * @param mcb 
 * @param vector -> it's the vector of priority queues
 * @param value -> the value to update - if the vector is empty, the value is set to -1
 */
void update_max_priority(Queue* vector, uint8_t* value){
    uint8_t i = 0;
    for (; i<MIN_PRIORITY; i++) {
        if(!empty(&(vector[i]))){
            *value = i;
            return;
        }
    }
    *value = -1;
}

void priority_inheritance(MCB* mcb){
    if(RUNNING->priority < mcb->max_priority_owners){
        // there is at least one task with that priority
        TaskTCB* task = dynamicList_searchPriotiy(mcb->owners, mcb->max_priority_owners);
        task->priority = RUNNING->priority;
    }
}

/**
 * @brief search a specific task with the given priority in a list
 * 
 * @param list 
 * @param p priority
 * @return TaskTCB* 
 */
TaskTCB* dynamicList_searchPriotiy(dynamicList_t* list, uint32_t p){
    uint32_t address;
    for(int i = 0; i < list->size; i++){
        address = list->base + i * sizeof(uint32_t);
        if(((TaskTCB*)address)->priority == p){
            return (TaskTCB*)address;
        }
    }
    // item not found
    return NULL;
}