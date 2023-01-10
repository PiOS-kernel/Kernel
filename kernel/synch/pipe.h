#ifndef PIPE_H
#define PIPE_H

#include<stdbool.h>

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
