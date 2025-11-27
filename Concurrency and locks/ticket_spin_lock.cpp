#include "src.hpp"

#define _GNU_SOURCE
      
void init(lock_t* lock)
{
    // Initialize both ticket and turn counters to 0.
    // 'ticket' is the next ticket to be given out.
    // 'turn' is the ticket number currently being served.
    lock->ticket = 0;
    lock->turn = 0;
}

void lock(lock_t* lock)
{
    // 1. Atomically get a ticket number.
    // __sync_fetch_and_add returns the *old* value of lock->ticket
    // *before* adding 1 to it. This is our unique ticket number.
    int my_turn = __sync_fetch_and_add(&(lock->ticket), 1);
    
    // 2. Spin (busy-wait) until the "now serving" number (lock->turn)
    // matches our ticket number (my_turn).
    while (lock->turn != my_turn) {
        // This is a busy-wait.
        // The CPU continuously checks this condition.
        // This is why it's called a "spin" lock.
        // On some architectures or with optimization, this
        // might be where a failure occurs if lock->turn is not volatile.
    }
    // Our turn has arrived, we now hold the lock.
}

void unlock(lock_t* lock)
{   
    // 3. We are done with the critical section.
    // Atomically increment the "now serving" counter to let
    // the next waiting thread (whose ticket is my_turn + 1)
    // exit its spin loop.
    __sync_fetch_and_add(&(lock->turn), 1);
}

void destroy(lock_t* lock)
{

}

