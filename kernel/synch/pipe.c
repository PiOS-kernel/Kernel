#include "pipe.h"
#include "../task/task.h"
#include "../utils/utils.h"
#include "../heap/malloc.h"
#include "../syscalls/syscalls.h"

//initialize to 0 the pipe fileds 
PIPE *new_pipe(int lenght, uint32_t msg_size){
    PIPE * pipe = (PIPE*) alloc(sizeof(PIPE));
    pipe->start = 0;
    pipe->end = 0;
    pipe->current_load = 0;
    pipe->write_mutex = mutex_init();
    pipe->read_mutex = mutex_init();
    pipe->waiting_on_read = NULL;
    pipe->waiting_on_write = NULL;
    pipe->msg_size = msg_size;
    pipe->pipe_size = lenght;
    pipe->messages = (char*) alloc(lenght*msg_size); 
    return pipe;
}

//used to write a message in the pipe
void pub_msg(PIPE *pipe, void *msg){
    synch_wait(pipe->write_mutex);                  //try to acquire the mutex for writing
    if(pipe->current_load >= pipe->pipe_size){      //check if there is room left for another message
        wait_writing(pipe);                         //the task should wait untill a task read from the pipe
    }    
    if (pipe->end == pipe->pipe_size*pipe->msg_size){    //check if the circular index is going out of bound
        pipe->end = 0;
    }
    memcpy(msg, (uint8_t*) &pipe->messages[pipe->end], pipe->msg_size);  //add the message and update the end indexes and current load counter
    pipe->end += pipe->msg_size;
    pipe->current_load++;
    unlock_reading(pipe);               //awake readers
    synch_post(pipe->write_mutex);      //free the mutex                  
}

//used to read the first message of the pipe and saving it in msg
void read_msg(PIPE *pipe, void *msg){
    synch_wait(pipe->read_mutex);                                           //try to acquire the mutex for reading
    if (pipe->current_load == 0){                                           //check if the pipe is empty
        wait_reading(pipe);                             //the task should wait for the pipe to have a message
    }
    memcpy((uint8_t*) &pipe->messages[pipe->start], (uint8_t*) msg, pipe->msg_size);                         //save in msg the first message of the pipe
    memset((uint8_t*) &(pipe->messages[pipe->start]),0,sizeof(pipe->msg_size));     //then delete the message just read from the pipe
    pipe->start = (pipe->start+pipe->msg_size) % pipe->pipe_size;                   //update the start index and current load counter
    pipe->current_load--;
    unlock_writing(pipe);                                                   //awake writers
    synch_post(pipe->read_mutex);                                           //free the mutex           
}

void wait_reading(PIPE *pipe) {
    pipe->waiting_on_read = RUNNING;                 //the task is saved in the pipe as the one waiting to write
    SHOULD_WAIT = 1;                                 //and needs to wait
    yield();                                          
}

void wait_writing(PIPE *pipe){
    pipe->waiting_on_write = RUNNING;                //the task is saved in the pipe as the one waiting to read
    SHOULD_WAIT = 1;                                 //and needs to wait
    yield();                                    
}

void unlock_reading(PIPE* pipe){
    if(pipe->waiting_on_read != NULL){                                      //if there is a task waiting to read
        enqueue(&READY_QUEUES[pipe->waiting_on_read->priority], pipe->waiting_on_read);     // put it back on ready
        pipe->waiting_on_read = NULL;                                       //and free the pointer
    }
}
void unlock_writing(PIPE* pipe){
    if(pipe->waiting_on_write != NULL){         //if there is a task waiting to write
        enqueue(&READY_QUEUES[pipe->waiting_on_write->priority], pipe->waiting_on_write);  //put the task back on ready
        pipe->waiting_on_write = NULL;          //free the pointer
    }
}
