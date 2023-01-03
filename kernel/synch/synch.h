#ifndef SYNCH_H
#define SYNCH_H

#include <stdint.h>
#include <stddef.h>
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
} dynamicList_t;

typedef struct MCB{
    uint32_t lock;
    void* owners; // pointer to an owner or a list of owners
    Lock_t type;
    uint32_t count;
} MCB;

MCB* mutex_init();
MCB* semaphore_init(uint32_t c);
void synch_wait(MCB* lock);
void synch_post(MCB* lock);

// dynamic list utilities
void dynamicList_init(dynamicList_t* list, uint8_t size);
uint8_t dynamicList_add(dynamicList_t* list, uint32_t item);
uint8_t dynamicList_remove(dynamicList_t* list, uint32_t item);
uint32_t* dynamicList_search(dynamicList_t* list, uint32_t item);

extern void enable_interrupts(void);
extern void disable_interrupts(void);

#endif
