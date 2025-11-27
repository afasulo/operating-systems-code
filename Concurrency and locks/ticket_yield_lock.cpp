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
    int my_turn = __sync_fetch_and_add(&(lock->ticket), 1);
    
    // 2. Wait until our number is called.
    //
    // We MUST use __sync_fetch_and_add() here to prevent
    // the compiler from creating an infinite loop.
    while (__sync_fetch_and_add(&(lock->turn), 0) != my_turn) {
        // Yield the CPU to another thread
        sched_yield();
    }
    // Our turn has arrived.g
}

void unlock(lock_t* lock)
{
    // 3. We are done with the critical section.
    // Atomically increment the "now serving" counter to let
    // the next waiting thread (whose ticket is my_turn + 1)
    // exit its wait loop.
    __sync_fetch_and_add(&(lock->turn), 1);
}

void destroy(lock_t* lock)
{

}

