# Memory Virtualization

This folder explores how operating systems implement virtual memory through address translation and page replacement.

## What's Inside

**translation.cpp** - Handles virtual-to-physical address translation. Includes:
- Splitting virtual addresses into VPN (virtual page number) and offset
- TLB lookups for fast address translation
- Walking the page table when TLB misses occur
- Handling protection faults and invalid addresses

**replacement.cpp** - Implements page replacement algorithms for when physical memory is full:
- FIFO (First-In-First-Out) - evicts the oldest page
- LRU (Least Recently Used) - evicts the page that hasn't been accessed in the longest time
- Clock algorithm - approximates LRU with a reference bit

## Building

Run the compile script:
```bash
./compile.sh
```

## Key Concepts

This code simulates what happens inside the Memory Management Unit (MMU). The TLB acts as a cache for address translations, and when physical memory fills up, the OS needs to decide which pages to evict. Different replacement policies have different tradeoffs - FIFO is simple but can evict frequently-used pages, while LRU performs better but requires tracking access times.
