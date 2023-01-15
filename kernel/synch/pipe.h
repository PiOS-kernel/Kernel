#ifndef PIPE_H
#define PIPE_H

#include "synch.h"

#define MESSAGE_SIZE 64
#define PIPE_SIZE 4

//struct to create a MESSAGE type
typedef struct MESSAGE {
  char data[MESSAGE_SIZE];
} MESSAGE;

//struct defining the pipe as a ciclic message queue 
typedef struct PIPE {
  int start;
  int end;
  int current_load;
  MESSAGE messages[PIPE_SIZE];
  MCB *read_mutex;
  MCB *write_mutex;
  Queue waiting_on_read;
  TaskTCB *waiting_on_write;
} PIPE;

void init_pipe(PIPE *pipe);
void pub_msg(PIPE *pipe, MESSAGE *msg);
void read_msg(PIPE *pipe, MESSAGE *msg);
void wait_reading(PIPE *pipe);
void wait_writing(PIPE *pipe);
void unlock_reading(PIPE *pipe);
void unlock_writing(PIPE *pipe);


#endif
