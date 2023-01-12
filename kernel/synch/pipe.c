#include "pipe.h"
#include "../task/task.h"
#include "../utils/utils.h"

//initialize to 0 the pipe fileds 
void init_pipe(PIPE *pipe){
    pipe->start = 0;
    pipe->end = 0;
    pipe->current_load = 0;
    pipe->semaphore = semaphore_init(WAITING_SIZE);
}

//used to add a message in the pipe
bool pub_msg(PIPE *pipe, MESSAGE *msg){
    synch_post(pipe->semaphore);
    if(pipe->current_load < PIPE_SIZE){ //check if therre is room left for another message
        if (pipe->end == PIPE_SIZE){    //check if the circular index is going out of bound
            pipe->end = 0;
        }
        pipe->messages[pipe->end] = *msg;  //add the message and update the end index and current load counter
        pipe->end++;
        pipe->current_load++;
        unlock(pipe);                   //if the message is written if there is a task waiting for it
        return true;                    // the task should be unlocked
    }else {
        return false;                   //return false if there is no room left
    }
}

//used to read the first message of the pipe and saving in in msg
bool read_msg(PIPE *pipe, MESSAGE *msg){
    synch_wait(pipe->semaphore);
    if (pipe->current_load > 0){
        *msg = pipe->messages[pipe->start];                       //save in msg the first message of the pipe
        memset((uint8_t*) &(pipe->messages[pipe->start]),0,sizeof(MESSAGE));    //then delete the message just read
        pipe->start = (pipe->start+1) % PIPE_SIZE;               //update the start index and current load counter
        pipe->current_load--;
        return true;
    }else {                         
        return false;                   //return false if the pipe is empty
    }
}
void wait(PIPE *pipe) {

}

void unlock(PIPE* pipe){

}
