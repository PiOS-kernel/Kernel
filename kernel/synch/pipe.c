#include <pipe.h>
#include <task.h>

//initialize to 0 the pipe fileds 
void init_pipe(PIPE *pipe){
    pipe->start = 0;
    pipe->end = 0;
    pipe->current_load = 0;
}

//used to add a message in the pipe
bool pub_msg(PIPE *pipe, MESSAGE *msg){
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
    if (pipe->current_load > 0){
        *msg = pipe->messages[pipe->start];                       //save in msg the first message of the pipe
        memset(pipe->messages[pipe->start],0,sizeof(MESSAGE));    //then delete the message just read
        pipe->start = (pipe->start+1) % PIPE_SIZE;               //update the start index and current load counter
        pipe->current_load--;
        return true;
    }else {
        wait(pipe);                     //if the pipe is empty the task shoudld wait untill someone writes on it                         
        return false;                   //return false if the pipe is empty
    }
}
void wait() {
    enqueue(&WAITING_QUEUES[RUNNING->priority],RUNNING);    //the task is placed in the waiting queue
    yield();                                                //and yields the cpu 
}

void unlock(PIPE* pipe){
    
}
