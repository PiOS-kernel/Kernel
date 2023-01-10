#ifndef PIPE_H
#define PIPE_H

#include<stdbool.h>

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
} PIPE;

void init_pipe(PIPE *pipe);
bool pub_msg(PIPE *pipe, MESSAGE *msg);
bool read_msg(PIPE *pipe, MESSAGE *msg);
void wait();
void unlock(PIPE *pipe);


#endif
