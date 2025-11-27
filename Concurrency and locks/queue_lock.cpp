#include "src.hpp"
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define _GNU_SOURCE

void queue_destroy(queue_t& queue);

// Singal handler for SIGUSR1
// This handler must be registered for sigwait() to work correctly so it can be empty here
void my_sig_handler(int signum)
{
    // kcik rocks. 
}


// initilize the queue lock
void init(lock_t* lock)
{
    lock->guard = 0; // 0 = unlocked, 1 = locked
    lock->flag = 0;  // 0 = lock free, 1 = lock held
    
    // queue_init is a C++ function taking a reference (queue_t&)
    queue_init(lock->queue);

    // This is the *only* signal setup needed for sigwait.
    // We must BLOCK SIGUSR1 for all threads.
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
        perror("pthread_sigmask");
        exit(EXIT_FAILURE);
    }

}



//acquires the lock
void lock(lock_t* lock)
{
    // 1. Acquire the guard (spin lock) to protect internal data
    while (__sync_lock_test_and_set(&(lock->guard), 1) == 1) {
        sched_yield(); // Yield while spinning
    }

    // --- Guarded Critical Section ---
    if (lock->flag == 0) {
        // 2a. Lock is free. Take it.
        lock->flag = 1;
        __sync_lock_release(&(lock->guard)); // Release guard
    } else {
        // 2b. Lock is held. Add self to wait queue and sleep.
        queue_add(lock->queue, pthread_self());
        __sync_lock_release(&(lock->guard)); // Release guard *before* sleeping

        // Prepare to wait for SIGUSR1
        sigset_t wait_mask;
        sigemptyset(&wait_mask);
        sigaddset(&wait_mask, SIGUSR1);

        int sig_received;
        
        // sigwait atomically unblocks SIGUSR1 and waits for it.
        // This solves the "lost wakeup" race condition.
        sigwait(&wait_mask, &sig_received);
        
        // When we wake up, we own the lock.
    }
    // --- End Guarded Critical Section ---

}

// releases the queue lock
void unlock(lock_t* lock)
{
    // 1. Acquire the guard (spin lock)
    while (__sync_lock_test_and_set(&(lock->guard), 1) == 1) {
        sched_yield();
    }

    // --- Guarded Critical Section ---
    if (queue_empty(lock->queue)) {
        // 2a. No threads are waiting. Just release the lock.
        lock->flag = 0;
    } else {
        // 2b. Threads are waiting. Wake one up.
        pthread_t next_thread = queue_remove(lock->queue);
        pthread_kill(next_thread, SIGUSR1);
    }
    __sync_lock_release(&(lock->guard)); // Release guard
    // --- End Guarded Critical Section ---
}

void destroy(lock_t* lock)
{
    // queue_destroy is C++ function that takes queue_t&
    queue_destroy(lock->queue);
}



