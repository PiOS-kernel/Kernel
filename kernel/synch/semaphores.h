#ifndef SYNCH_H
#define SYNCH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../task/task.h"

#define MAX_SIZE_WAITING 8

typedef enum {
  MUTEX,
  SEMAPHORE_BIN,
  SEMAPHORE_INT
} Lock_t ;

typedef struct{
  uint32_t base;
  uint8_t size; // items - max 256 items - items are 32-bit words
  uint8_t allignment[3];
} dynamicList_t;

typedef struct MCB{
    uint32_t lock;
    dynamicList_t* owners; // holds the address of the owner or a list of owners
    uint8_t max_priority_owners;
    Lock_t type;
    uint32_t count;
    Queue waiting[MIN_PRIORITY]; // vector of priority queues
    uint8_t max_priority_waiting;
} MCB;

MCB* mutex_init();
MCB* semaphore_init(uint32_t c);
void synch_wait(MCB* lock);
void synch_post(MCB* lock);

// dynamic list utilities
dynamicList_t* dynamicList_init(uint8_t size);
uint8_t dynamicList_add(dynamicList_t* list, uint32_t item);
uint8_t dynamicList_remove(dynamicList_t* list, uint32_t item);
uint32_t* dynamicList_search(dynamicList_t* list, uint32_t item);

void priority_inheritance(MCB* mcb);
void update_max_waitPriority(MCB* mcb);
void update_max_ownerPriority(MCB* mcb);
TaskTCB* dynamicList_searchPriority(dynamicList_t* list, uint32_t p);

extern void enable_interrupts(void);
extern void disable_interrupts(void);

#endif
