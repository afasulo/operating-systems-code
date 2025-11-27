#include <semaphore.h>
#include "src.hpp"

void* producer_thread(void* args)
{
    data_t* data = (data_t*)args;
    buffer_t* buf = data->buf;
    //int val = data->val;
    // producers waits if the sem_empty > 0, if it is, decrement then proceed and write to buf
    // producer waits here if the buffer is full
    sem_wait(buf->sem_empty);

    //protects the critical section
    pthread_mutex_lock(&buf->mutex);
    put(buf, data->val);
    pthread_mutex_unlock(&buf->mutex);

    // after writting, increment the sem_full counter indicating there is one more item to consume
    sem_post(buf->sem_full);

    return NULL;
}

void* consumer_thread(void* args)
{
    buffer_t* buf = (buffer_t*)args;
    int* result = new int;

    // so if sem_full is 0, then wait here until it is >0 (AKA producer executed and incremented sem_post(sem_full))
    // if sem_full > 0 then decrment and consume
    sem_wait(buf->sem_full);

    //protect critcal section
    pthread_mutex_lock(&buf->mutex);
    get(buf, result);
    pthread_mutex_unlock(&buf->mutex);

    // increment sem_empty ?
    sem_post(buf->sem_empty);

    return result;
}
