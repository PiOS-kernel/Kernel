#ifndef SYNCH_H
#define SYNCH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../task/task.h"

typedef enum {
  MUTEX,
  SEMAPHORE_BIN,
  SEMAPHORE_INT
} Lock_t ; 

typedef struct MCB{
    uint32_t lock;
    TaskTCB* owner;
    Lock_t type;
    uint32_t count;
} MCB;

MCB* mutex_init();
MCB* semaphore_init(uint32_t c);
void synch_wait(MCB* lock);
void synch_post(MCB* lock);

extern void enable_interrupts(void);
extern void disable_interrupts(void);

#endif
