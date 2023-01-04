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

//structures needed for message queues

#define MESSAGE_SIZE 64
#define QUEUE_SIZE 4

//struct to create a MESSAGE type
typedef struct MESSAGE {
  char data[MESSAGE_SIZE];
} MESSAGE;

//struct defining the ciclic message queue 
typedef struct QUEUE {
  int start;
  int end;
  int current_load;
  MESSAGE messages[QUEUE_SIZE];
} QUEUE;

void init_queue(QUEUE *queue);
bool pub_msg(QUEUE *queue, MESSAGE *msg);
bool read_msg(QUEUE *queue, MESSAGE *msg);


#endif
