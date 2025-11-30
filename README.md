# Operating Systems Code

This repository contains implementations of fundamental operating systems concepts, organized into five main areas.

## Directory Structure

### kernel-security/
**Linux kernel modules for security monitoring.** Includes:
- `process_monitor.c` — Kprobe-based process creation monitor (fork/execve hooking)
- `syscall_integrity.c` — Syscall table integrity checker for rootkit detection

Demonstrates kernel-level introspection, the foundation for VMI (Virtual Machine Introspection) and host-based intrusion detection.

### processes and scheduling/
Process management and CPU scheduling algorithms. Covers process creation with `fork()`, process hierarchies, and various scheduling strategies including priority scheduling, FCFS, SJF, and Round Robin.

### Concurrency and locks/
Synchronization primitives and lock implementations. Includes ticket-based locks (spin and yield variants), queue locks, semaphore-based locks, and a parallel Monte Carlo pi calculator demonstrating practical concurrency patterns.

### memory virtualization/
Virtual memory implementation with address translation and page replacement. Simulates MMU behavior with TLB lookups, page table walks, and replacement algorithms (FIFO, LRU, Clock).

### Producer and dining Philosophy/
Classic concurrency problems and their solutions. Implements producer-consumer using both condition variables and semaphores, plus dining philosophers solutions that prevent deadlock.

## Building

Each directory contains its own compile script:
```bash
cd <directory>
./compile.sh
```

Alternatively, use CMake in any directory.

## Topics Covered

- **Kernel Development**: Loadable kernel modules, kprobes, syscall hooking
- **Security Monitoring**: Process auditing, rootkit detection, integrity checking
- **Process Management**: fork, waitpid, process hierarchies
- **CPU Scheduling**: Priority, FCFS, SJF, Round Robin
- **Synchronization**: Locks, semaphores, condition variables
- **Concurrency Patterns**: Parallel computation, shared state protection
- **Virtual Memory**: Address translation, TLB, page tables, page replacement
- **Classic Problems**: Producer-consumer, dining philosophers, deadlock prevention

## Purpose

These implementations demonstrate core OS concepts through working code. Each directory's README contains additional details about the specific programs and what they illustrate.
