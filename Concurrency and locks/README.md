# Concurrency and Locks

This folder contains implementations of various synchronization primitives and a multithreaded application that uses them.

## What's Inside

**compute_pi.cpp** - A parallel Monte Carlo simulation that estimates pi by randomly sampling points. Multiple threads divide up the work, and locks protect the shared sum when threads combine their results.

**ticket_spin_lock.cpp** - Implements a fair ticket-based spinlock (like taking a number at the deli). Ensures threads acquire the lock in the order they requested it, preventing starvation.

**ticket_yield_lock.cpp** - Similar to the spin lock, but yields the CPU instead of busy-waiting when the lock isn't available. More efficient when contention is high.

**queue_lock.cpp** - A lock implementation using a queue structure to manage waiting threads.

**semaphore_lock.cpp** - Uses semaphores as the underlying synchronization mechanism for locks.

## Building

Compile everything with:
```bash
./compile.sh
```

## Notes

The pi calculation demonstrates a key pattern in parallel programming - do as much work as possible without synchronization (each thread counts its own points), then only use locks for the brief moment when updating shared state. The different lock implementations show various tradeoffs between fairness, CPU usage, and complexity.
