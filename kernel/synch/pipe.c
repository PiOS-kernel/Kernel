#include <pipe.h>

//initialize to 0 the queue fileds 
void init_queue(QUEUE *queue){
    queue->start = 0;
    queue->end = 0;
    queue->current_load = 0;
}

//used to add a message in the queue
bool pub_msg(QUEUE *queue, MESSAGE *msg){
    if(queue->current_load < QUEUE_SIZE){ //check if therre is room left for another message
        if (queue->end == QUEUE_SIZE){    //check if the circular index is going out of bound
            queue->end = 0;
        }
        queue->messages[queue->end] = *msg;  //add the message and update the end index and current load counter
        queue->end++;
        queue->current_load++;
        return true;
    }else {
        return false;                   //return false if there is no room left
    }
}

//used to read the first message of the queue and saving in in msg
bool read_msg(QUEUE *queue, MESSAGE *msg){
    if (queue->current_load > 0){
        *msg = queue->messages[queue->start];                       //save in msg the first message of the queue
        memset(queue->messages[queue->start],0,sizeof(MESSAGE));    //then delete the message just read
        queue->start = (queue->start+1) % QUEUE_SIZE;               //update the start index and current load counter
        queue->current_load--;
        return true;
    }else {
        return false;                   //return false if the queue is empty
    }
}