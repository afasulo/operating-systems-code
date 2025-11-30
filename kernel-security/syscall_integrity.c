/*
 * syscall_integrity.c - Syscall Table Integrity Validator
 *
 * This module demonstrates how to detect syscall table hooking,
 * a common technique used by kernel rootkits to hide malicious activity.
 *
 * How rootkits hook syscalls:
 *   1. Locate the syscall table (sys_call_table)
 *   2. Disable write protection (CR0.WP bit)
 *   3. Replace syscall pointer with malicious function
 *   4. Malicious function filters results (hides files, processes, etc.)
 *
 * How we detect this:
 *   1. Read known-good syscall addresses from /proc/kallsyms at load time
 *   2. Periodically compare current syscall table against baseline
 *   3. Alert if any addresses have changed
 *
 * Limitations:
 *   - Sophisticated rootkits may hide from kallsyms too
 *   - Some legitimate software (security tools, tracers) may hook syscalls
 *   - This is educational - production tools use more robust methods
 *
 * Author: Adam Fasulo
 * License: GPL v2
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Fasulo");
MODULE_DESCRIPTION("Syscall table integrity checker");
MODULE_VERSION("1.0");

/* Check interval in seconds */
#define CHECK_INTERVAL_SEC 30

/* Number of critical syscalls to monitor */
#define NUM_MONITORED_SYSCALLS 10

/* Syscall numbers to monitor (x86_64) */
static const int monitored_syscalls[NUM_MONITORED_SYSCALLS] = {
    __NR_read,          /* 0 - File read */
    __NR_write,         /* 1 - File write */
    __NR_open,          /* 2 - File open */
    __NR_getdents64,    /* 217 - Directory listing (hide files) */
    __NR_kill,          /* 62 - Process signaling */
    __NR_connect,       /* 42 - Network connect (hide connections) */
    __NR_recvfrom,      /* 45 - Network receive */
    __NR_execve,        /* 59 - Execute program */
    __NR_ptrace,        /* 101 - Process trace (anti-debugging) */
    __NR_init_module,   /* 175 - Load kernel module */
};

/* Baseline addresses captured at module load */
static unsigned long baseline_addrs[NUM_MONITORED_SYSCALLS];

/* Pointer to syscall table */
static unsigned long *syscall_table = NULL;

/* Timer for periodic checks */
static struct timer_list check_timer;

/*
 * find_syscall_table - Locate the syscall table in kernel memory
 *
 * Uses kallsyms_lookup_name to find sys_call_table symbol.
 * Note: kallsyms_lookup_name was unexported in kernel 5.7+
 * This code includes a fallback for newer kernels.
 */
static unsigned long *find_syscall_table(void)
{
    unsigned long *table = NULL;
    
    /* Try kallsyms first */
    table = (unsigned long *)kallsyms_lookup_name("sys_call_table");
    
    if (!table) {
        pr_warn("SYSCALL_CHK: Could not find sys_call_table via kallsyms\n");
        pr_warn("SYSCALL_CHK: On kernel 5.7+, kallsyms_lookup_name is restricted\n");
        return NULL;
    }
    
    return table;
}

/*
 * capture_baseline - Record current syscall addresses
 *
 * Called once at module load to establish the "known good" state.
 * Any deviation from this baseline indicates potential tampering.
 */
static void capture_baseline(void)
{
    int i;
    
    for (i = 0; i < NUM_MONITORED_SYSCALLS; i++) {
        baseline_addrs[i] = syscall_table[monitored_syscalls[i]];
        pr_debug("SYSCALL_CHK: Baseline syscall %d = 0x%lx\n",
                 monitored_syscalls[i], baseline_addrs[i]);
    }
    
    pr_info("SYSCALL_CHK: Captured baseline for %d syscalls\n", 
            NUM_MONITORED_SYSCALLS);
}

/*
 * check_integrity - Compare current syscall table against baseline
 *
 * Returns the number of modified syscalls detected.
 * In a production system, this would trigger an alert.
 */
static int check_integrity(void)
{
    int i;
    int violations = 0;
    unsigned long current_addr;
    
    for (i = 0; i < NUM_MONITORED_SYSCALLS; i++) {
        current_addr = syscall_table[monitored_syscalls[i]];
        
        if (current_addr != baseline_addrs[i]) {
            violations++;
            pr_alert("SYSCALL_CHK: [ALERT] Syscall %d MODIFIED!\n",
                     monitored_syscalls[i]);
            pr_alert("SYSCALL_CHK:   Expected: 0x%lx\n", baseline_addrs[i]);
            pr_alert("SYSCALL_CHK:   Found:    0x%lx\n", current_addr);
            
            /*
             * In production, you might:
             *   - Send alert to SIEM
             *   - Trigger incident response
             *   - Attempt to restore original handler
             *   - Panic the system (nuclear option)
             */
        }
    }
    
    return violations;
}

/*
 * timer_callback - Periodic integrity check
 *
 * Called every CHECK_INTERVAL_SEC seconds to verify syscall table.
 */
static void timer_callback(struct timer_list *t)
{
    int violations;
    
    violations = check_integrity();
    
    if (violations == 0) {
        pr_debug("SYSCALL_CHK: Integrity check passed\n");
    } else {
        pr_alert("SYSCALL_CHK: Integrity check FAILED! %d violations\n", 
                 violations);
    }
    
    /* Reschedule timer */
    mod_timer(&check_timer, jiffies + (CHECK_INTERVAL_SEC * HZ));
}

/*
 * Module initialization
 */
static int __init syscall_chk_init(void)
{
    pr_info("SYSCALL_CHK: Initializing syscall integrity checker\n");
    
    /* Find syscall table */
    syscall_table = find_syscall_table();
    if (!syscall_table) {
        pr_err("SYSCALL_CHK: Cannot locate syscall table, aborting\n");
        return -EFAULT;
    }
    
    pr_info("SYSCALL_CHK: Found sys_call_table at 0x%px\n", syscall_table);
    
    /* Capture baseline */
    capture_baseline();
    
    /* Initial integrity check */
    check_integrity();
    
    /* Setup periodic timer */
    timer_setup(&check_timer, timer_callback, 0);
    mod_timer(&check_timer, jiffies + (CHECK_INTERVAL_SEC * HZ));
    
    pr_info("SYSCALL_CHK: Module loaded, checking every %d seconds\n",
            CHECK_INTERVAL_SEC);
    
    return 0;
}

/*
 * Module cleanup
 */
static void __exit syscall_chk_exit(void)
{
    /* Cancel timer */
    del_timer_sync(&check_timer);
    
    /* Final integrity check */
    pr_info("SYSCALL_CHK: Final integrity check:\n");
    check_integrity();
    
    pr_info("SYSCALL_CHK: Module unloaded\n");
}

module_init(syscall_chk_init);
module_exit(syscall_chk_exit);

