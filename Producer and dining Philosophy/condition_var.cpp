#include "src.hpp"

void* producer_thread(void* args)
{
    data_t* data = (data_t*)args;
    buffer_t* buf = data->buf;

    // lock the thread while we write to it
    pthread_mutex_lock(&buf->mutex);
    // while buf is full
    while (buf->size == buf->limit){
        // does 3 things atomically. unlock the mutex, puts thread to sleep, and relocks when thread wakes up
        pthread_cond_wait(&buf->empty, &buf->mutex);
    }

    // buf wasnt full. put data in buf
    put(buf, data->val);

    // mutex is now full. signal that buf is full
    pthread_cond_signal(&buf->full);
    // unlock the mutex for buf
    pthread_mutex_unlock(&buf->mutex);
    
    return NULL;
}

void* consumer_thread(void* args)
{
    buffer_t* buf = (buffer_t*)args;
    int* result = new int;

    // lock mutex cause we gon' eat
    pthread_mutex_lock(&buf->mutex);

    // while buf is empty
    while (buf->size == 0) {
        // unlock mutex, put thread to sleep and acquire the mutex again when we wake up
        pthread_cond_wait(&buf->full, &buf->mutex);
    }

    // buf is full so eat it
    get(buf, result);

    // We just ate the whole buf so signal to producer that he can cook
    pthread_cond_signal(&buf->empty);
    //give up mutex
    pthread_mutex_unlock(&buf->mutex);

    return result;
}
