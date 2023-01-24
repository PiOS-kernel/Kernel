#include "channels.h"
#include "../task/task.h"
#include "../utils/utils.h"
#include "../heap/malloc.h"
#include "../syscalls/syscalls.h"

//initialize to 0 the channel fileds 
Channel *new_pipe(int lenght, uint32_t msg_size){
    Channel *channel = (Channel*) alloc(sizeof(Channel));
    channel->start = 0;
    channel->end = 0;
    channel->current_load = 0;
    channel->write_mutex = mutex_init();
    channel->read_mutex = mutex_init();
    channel->waiting_on_read = NULL;
    channel->waiting_on_write = NULL;
    channel->msg_size = msg_size;
    channel->channel_size = lenght;
    channel->messages = (uint8_t*) alloc(lenght*msg_size); 
    return channel;
}

//used to write a message in the channel
void pub_msg(Channel *channel, void *msg){
    synch_wait(channel->write_mutex);                  //try to acquire the mutex for writing
    if(channel->current_load >= channel->channel_size){      //check if there is room left for another message
        wait_writing(channel);                         //the task should wait untill a task read from the channel
    }    
    if (channel->end == channel->channel_size * channel->msg_size){    //check if the circular index is going out of bound
        channel->end = 0;
    }
    memcpy(msg, (uint8_t*) &channel->messages[channel->end], channel->msg_size);  //add the message and update the end indexes and current load counter
    channel->end += channel->msg_size;
    channel->current_load++;
    unlock_reading(channel);               //awake readers
    synch_post(channel->write_mutex);      //free the mutex                  
}

//used to read the first message of the channel and saving it in msg
void read_msg(Channel *channel, void *msg){
    synch_wait(channel->read_mutex);                                           //try to acquire the mutex for reading
    if (channel->current_load == 0){                                           //check if the channel is empty
        wait_reading(channel);                             //the task should wait for the channel to have a message
    }
    memcpy((uint8_t*) &channel->messages[channel->start], (uint8_t*) msg, channel->msg_size);                         //save in msg the first message of the channel
    memset((uint8_t*) &(channel->messages[channel->start]),0,sizeof(channel->msg_size));     //then delete the message just read from the channel
    channel->start = (channel->start+channel->msg_size) % channel->channel_size;                   //update the start index and current load counter
    channel->current_load--;
    unlock_writing(channel);                                                   //awake writers
    synch_post(channel->read_mutex);                                           //free the mutex           
}

void wait_reading(Channel *channel) {
    channel->waiting_on_read = RUNNING;                 //the task is saved in the channel as the one waiting to write
    SHOULD_WAIT = 1;                                 //and needs to wait
    yield();                                          
}

void wait_writing(Channel *channel){
    channel->waiting_on_write = RUNNING;                //the task is saved in the channel as the one waiting to read
    SHOULD_WAIT = 1;                                 //and needs to wait
    yield();                                    
}

void unlock_reading(Channel* channel){
    if(channel->waiting_on_read != NULL){                                      //if there is a task waiting to read
        enqueue(&READY_QUEUES[channel->waiting_on_read->priority], channel->waiting_on_read);     // put it back on ready
        channel->waiting_on_read = NULL;                                       //and free the pointer
    }
}
void unlock_writing(Channel* channel){
    if(channel->waiting_on_write != NULL){         //if there is a task waiting to write
        enqueue(&READY_QUEUES[channel->waiting_on_write->priority], channel->waiting_on_write);  //put the task back on ready
        channel->waiting_on_write = NULL;          //free the pointer
    }
}
