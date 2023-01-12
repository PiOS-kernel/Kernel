#include "pipe.h"
#include "../task/task.h"
#include "../utils/utils.h"

//initialize to 0 the pipe fileds 
void init_pipe(PIPE *pipe){
    pipe->start = 0;
    pipe->end = 0;
    pipe->current_load = 0;
    pipe->write_mutex = mutex_init();
    pipe->read_mutex = mutex_init();
    Queue_init(&pipe->waiting_on_read);
    Queue_init(&pipe->waiting_on_write);
}

//used to add a message in the pipe
bool pub_msg(PIPE *pipe, MESSAGE *msg){
    synch_wait(pipe->write_mutex);      //try to acquire the mutex for writing
    if(pipe->current_load < PIPE_SIZE){ //check if there is room left for another message
        if (pipe->end == PIPE_SIZE){    //check if the circular index is going out of bound
            pipe->end = 0;
        }
        pipe->messages[pipe->end] = *msg;  //add the message and update the end index and current load counter
        pipe->end++;
        pipe->current_load++;
        unlock_reading(pipe);               //awake readers
        synch_post(pipe->write_mutex);      //free the mutex
        return true;
    }                    
        wait_writing(pipe);             //the task should wait untill a task read from the pipe
        return false;                   //return false if the pipe was full
}

//used to read the first message of the pipe and saving in in msg
bool read_msg(PIPE *pipe, MESSAGE *msg){
    synch_wait(pipe->read_mutex);                                           //try to acquire the mutex for reading
    if (pipe->current_load > 0){                                            //check if the pipe is not empty
        *msg = pipe->messages[pipe->start];                                     //save in msg the first message of the pipe
        memset((uint8_t*) &(pipe->messages[pipe->start]),0,sizeof(MESSAGE));    //then delete the message just read
        pipe->start = (pipe->start+1) % PIPE_SIZE;                              //update the start index and current load counter
        pipe->current_load--;
        unlock_writing(pipe);                                                   //awake writers
        synch_post(pipe->read_mutex);                                           //free the mutex
        return true;
    }else {            
        wait_reading(pipe);             //the task wait for someone to write on the pipe            
        return false;                   //return false if the pipe was empty
    }
}
void wait_reading(PIPE *pipe) {
    enqueue(&pipe->waiting_on_read, RUNNING);       //the task is saved in the pipe waiting queue
    SHOULD_WAIT = 1;                                //and needs to wait
    PendSVTrigger();                                          
}

void wait_writing(PIPE *pipe){
    enqueue(&pipe->waiting_on_write, RUNNING);       //the task is saved in the pipe waiting queue
    SHOULD_WAIT = 1;                                 //and need to wait
    PendSVTrigger();                                    
}

void unlock_reading(PIPE* pipe){
    if(!empty(&pipe->waiting_on_read)){     //if there is some task waiting to read
        TaskTCB *unlocked_task = dequeue(&pipe->waiting_on_read);           //free one task from the queue
        unlink_task(unlocked_task);                                         //and put it back on ready
        enqueue(&READY_QUEUES[unlocked_task->priority], unlocked_task);
    }
}
void unlock_writing(PIPE* pipe){
    if(!empty(&pipe->waiting_on_read)){     //if there is some task waiting to write
        TaskTCB *unlocked_task = dequeue(&pipe->waiting_on_write);           //free one task from the queue
        unlink_task(unlocked_task);                                         //and put it back on ready
        enqueue(&READY_QUEUES[unlocked_task->priority], unlocked_task);
    }
}
