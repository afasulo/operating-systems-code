# Producer-Consumer and Dining Philosophers

This folder tackles classic concurrency problems using different synchronization approaches.

## What's Inside

**condition_var.cpp** - Solves the producer-consumer problem using condition variables and mutexes. Producers wait when the buffer is full, consumers wait when it's empty. Condition variables let threads sleep instead of spinning, and wake up exactly when they need to.

**semaphore.cpp** - An alternative producer-consumer implementation using semaphores to track empty and full slots in the buffer.

**circular.cpp** - Implements the dining philosophers problem. Five philosophers sit at a table, each needing two forks to eat. The solution prevents deadlock by having even-numbered philosophers pick up their right fork first, while odd-numbered ones go left first. This breaks the circular wait condition.

**hold_and_wait.cpp** - Another approach to dining philosophers that addresses the hold-and-wait deadlock condition.

## Building

```bash
./compile.sh
```

## The Problems

**Producer-Consumer**: How do you coordinate threads that produce data with threads that consume it, using a shared buffer? You need to ensure producers don't overflow the buffer and consumers don't try to read from an empty one.

**Dining Philosophers**: A classic deadlock scenario. If all philosophers grab their left fork simultaneously, they'll wait forever for the right fork. The key insight is breaking symmetry - not all philosophers can do the exact same thing at the same time.
