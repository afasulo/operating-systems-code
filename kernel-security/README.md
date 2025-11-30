# Kernel Security Modules

 These examples show how kernel-level introspection works — the foundation for tools like VMI (Virtual Machine Introspection) and rootkit detection.

## Modules

### 1. `process_monitor.c` - Process Creation Monitor
Hooks into the kernel to log all `fork()` and `execve()` calls. Demonstrates:
- Kprobe-based function hooking
- Accessing process credentials (UID, PID, comm)
- Kernel logging with `printk`

**Use case**: Detecting unauthorized process spawning, monitoring for suspicious binaries.

### 2. `syscall_integrity.c` - Syscall Table Validator
Validates that the syscall table hasn't been tampered with by comparing against known-good addresses. Demonstrates:
- Reading kernel symbols
- Detecting syscall hooking (common rootkit technique)
- Kernel module initialization/cleanup

**Use case**: Detecting rootkits that modify the syscall table to hide processes or files.

## Building

Requires kernel headers for your running kernel:

```bash
# Ubuntu/Debian
sudo apt install linux-headers-$(uname -r)

# Build modules
make

# Load module
sudo insmod process_monitor.ko

# Check kernel log
sudo dmesg | tail -20

# Unload module
sudo rmmod process_monitor
```

## Security Context

These modules demonstrate **defensive** techniques:
- **Process monitoring** is used by EDR/HIDS to detect malicious activity
- **Syscall integrity checking** detects a class of kernel rootkits

Understanding how these work also reveals **offensive** techniques:
- Attackers hook syscalls to hide files, processes, network connections
- Rootkits modify kernel structures to evade detection

This dual understanding is essential for security research and threat analysis.

## ⚠️ Warning

Kernel modules run with full system privileges. Bugs can crash your system. **I have a way to collect kernal panic crash data** If interested, contact me.
**Always test in a VM first.**

## References

- [Linux Kernel Module Programming Guide](https://sysprog21.github.io/lkmpg/)
- [Kprobes Documentation](https://www.kernel.org/doc/html/latest/trace/kprobes.html)
- [Syscall Table Hooking Explained](https://xcellerator.github.io/posts/linux_rootkits_01/)

