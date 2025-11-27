# Process Management and Scheduling

This folder contains implementations of basic process management and various CPU scheduling algorithms.

## What's Inside

**processes.cpp** - Creates a simple process hierarchy using `fork()`. Demonstrates how parent, child, and grandchild processes coordinate using `waitpid()` to ensure they execute in the correct order.

**scheduler.cpp** - Implements several classic scheduling algorithms:
- Priority scheduling (jobs with lower priority numbers run first)
- First-Come-First-Served (FCFS)
- Shortest Job First (SJF)
- Round Robin with time slicing

## Building

Use the provided build script:
```bash
./compile.sh
```

Or build with CMake if you prefer.

## What I Learned

Working through this helped me understand how operating systems create and manage processes, and how different scheduling strategies impact which processes run when. The priority scheduler was particularly interesting since it required sorting jobs while maintaining their original order as a tiebreaker.
