#include "src.hpp"

#define _GNU_SOURCE

void init(lock_t* lock)
{
    // Initialize the semaphore value. Since we're using it as a lock,
    // this is a binary semaphore, so S should be 1 (resource available).
    lock->S = 1;

    // Initialize the internal mutex that protects the counter S.
    pthread_mutex_init(&(lock->mutex), NULL);
}

void lock(lock_t* lock)
{
    // Loop until we successfully acquire the lock (decrement S).
    while (1) {
        // Lock the internal mutex to protect the variable S.
        pthread_mutex_lock(&(lock->mutex));

        if (lock->S > 0) {
            // Resource is available. Decrement S, release the mutex,
            // and break the loop. We now hold the lock.
            lock->S--;
            pthread_mutex_unlock(&(lock->mutex));
            break;
        } else {
            // Resource is not available. Release the mutex so another
            // thread (the unlocker) can increment S.
            pthread_mutex_unlock(&(lock->mutex));

            // Yield the CPU to be polite and avoid a pure busy-wait.
            sched_yield();
        }
    }
}

void unlock(lock_t* lock)
{
    // Lock the internal mutex to protect the variable S
    pthread_mutex_lock(&(lock->mutex));

    // Increment S to signal that the resource is available.
    lock->S++;

    // Release the internal mutex.
    pthread_mutex_unlock(&(lock->mutex));
}

void destroy(lock_t* lock)
{
    // Clean up the internal mutex.
    pthread_mutex_destroy(&(lock->mutex));
}
