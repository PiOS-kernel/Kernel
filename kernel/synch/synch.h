#ifndef SYNCH_H
#define SYNCH_H

#include <stdint.h>
#include <stddef.h>
#include "../task/task.h"

typedef struct MCB{
    uint32_t lock;
    TaskTCB* owner;
} MCB;

MCB* MCB_init(void);
void mutex_wait(MCB* lock);
void mutex_post(MCB* lock);

#endif