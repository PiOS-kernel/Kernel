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
    if(lock->lock == 0){
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
        while(!mutex_wait(lock));
    } else {
        while(!sem_wait(lock));
    }
    // priority inheritance ?
    // call context switch
    PendSVTrigger();
}

void synch_post(MCB* lock){
    if(lock->type == MUTEX){
        mutex_post(lock);
    } else {
        sem_post(lock);
    }
}

//message queue functions implementation

//initialize to 0 the queue fileds 
void init_queue(QUEUE *queue){
    queue->start = 0;
    queue->end = 0;
    queue->current_load = 0;
}

//used to add a message in the queue
bool pub_msg(QUEUE *queue, MESSAGE *msg){
    if(queue->current_load < QUEUE_SIZE){ //check if therre is room left for another message
        if (queue->end == QUEUE_SIZE){    //check if the circular index is going out of bound
            queue->end = 0;
        }
        queue->messages[queue->end] = *msg;  //add the message and update the end index and current load counter
        queue->end++;
        queue->current_load++;
        return true;
    }else {
        return false;                   //return false if there is no room left
    }
}

//used to read the first message of the queue and saving in in msg
bool read_msg(QUEUE *queue, MESSAGE *msg){
    if (queue->current_load > 0){
        *msg = queue->messages[queue->start];                       //save in msg the first message of the queue
        memset(queue->messages[queue->start],0,sizeof(MESSAGE));    //then delete the message just read
        queue->start = (queue->start+1) % QUEUE_SIZE;               //update the start index and current load counter
        queue->current_load--;
        return true;
    }else {
        return false;                   //return false if the queue is empty
    }
}