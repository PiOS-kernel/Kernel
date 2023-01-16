#ifndef PIPE_H
#define PIPE_H

#include "synch.h"

#define NEW_PIPE(lenght, msg_type) init_pipe(lenght, sizeof(msg_type))

//struct defining the pipe as a ciclic message queue 
typedef struct PIPE {
  int start;
  int end;
  int current_load;
  MCB *read_mutex;
  MCB *write_mutex;
  Queue waiting_on_read;
  TaskTCB *waiting_on_write;
  uint32_t msg_size;
  uint32_t pipe_size;
  char* messages;
} PIPE;

PIPE *init_pipe(int lenght, uint32_t msg_size);
void pub_msg(PIPE *pipe, void *msg); 
void read_msg(PIPE *pipe, void *msg);
void wait_reading(PIPE *pipe);
void wait_writing(PIPE *pipe);
void unlock_reading(PIPE *pipe);
void unlock_writing(PIPE *pipe);


#endif
