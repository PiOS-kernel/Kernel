#ifndef QUEUES_H
#define QUEUES_H

#include "semaphores.h"

#define NEW_CHANNEL(lenght, msg_type) new_channel(lenght, sizeof(msg_type))

// a ciclic message queue 
typedef struct Channel {
  int start;
  int end;
  int current_load;
  MCB *read_mutex;
  MCB *write_mutex;
  TaskTCB *waiting_on_read;
  TaskTCB *waiting_on_write;
  uint32_t msg_size;
  uint32_t channel_size;
  uint8_t* messages;
} Channel;

Channel *new_channel(int lenght, uint32_t msg_size);
void pub_msg(Channel *channel, void *msg); 
void read_msg(Channel *channel, void *msg);
void wait_reading(Channel *channel);
void wait_writing(Channel *channel);
void unlock_reading(Channel *channel);
void unlock_writing(Channel *channel);


#endif
