#ifndef SYNCH_H
#define SYNCH_H

#include <stdint.h>
#include <stddef.h>
#include "../task/task.h"

typedef struct MCB{
    uint32_t lock;
    TaskTCB* owner;
} MCB;

MCB* mutex_init(void);
void mutex_wait(MCB* lock);
void mutex_post(MCB* lock);

extern void enable_interrupts(void);
extern void disable_interrupts(void);

#endif
