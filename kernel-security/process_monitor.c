/*
 * process_monitor.c - Linux Kernel Module for Process Creation Monitoring
 *
 * This module uses kprobes to hook process creation events (do_fork/kernel_clone
 * and execve). It logs process creation with PID, UID, and command name.
 *
 * Security applications:
 *   - Host-based intrusion detection (HIDS)
 *   - Endpoint detection and response (EDR)
 *   - Forensic analysis and audit logging
 *   - Detecting unauthorized process spawning
 *
 * Build: make
 * Load: sudo insmod process_monitor.ko
 * View: sudo dmesg | grep "PROC_MON"
 * Unload: sudo rmmod process_monitor
 *
 * Author: Adam Fasulo
 * License: GPL v2
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/dcache.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Fasulo");
MODULE_DESCRIPTION("Process creation monitor using kprobes");
MODULE_VERSION("1.0");

/* Kprobe structures for hooking */
static struct kprobe kp_execve;
static struct kprobe kp_fork;

/*
 * get_process_path - Attempt to get the executable path of current process
 * 
 * Returns a pointer to a statically allocated buffer containing the path,
 * or "(unknown)" if the path cannot be determined.
 * 
 * Note: This is a simplified version. Production code would need
 * proper locking and memory management.
 */
static char *get_process_path(void)
{
    static char buf[256];
    struct file *exe_file;
    char *path = "(unknown)";
    
    if (current->mm && current->mm->exe_file) {
        exe_file = current->mm->exe_file;
        path = d_path(&exe_file->f_path, buf, sizeof(buf));
        if (IS_ERR(path))
            path = "(unknown)";
    }
    
    return path;
}

/*
 * execve_handler - Called before execve() syscall executes
 *
 * Logs the target binary being executed along with caller information.
 * This is where you'd implement allow/deny logic for application whitelisting.
 */
static int execve_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    const struct cred *cred = current_cred();
    
    /*
     * Log format: PROC_MON [EXEC] uid=X pid=X ppid=X comm=X
     * 
     * In production, you might:
     *   - Check against a whitelist of allowed binaries
     *   - Alert on execution from /tmp or other suspicious paths
     *   - Track process lineage for threat hunting
     */
    pr_info("PROC_MON [EXEC] uid=%d pid=%d ppid=%d comm=%s exe=%s\n",
            cred->uid.val,
            current->pid,
            current->real_parent->pid,
            current->comm,
            get_process_path());
    
    return 0;  /* Continue execution */
}

/*
 * fork_handler - Called when a new process is created
 *
 * Note: The exact function to hook depends on kernel version:
 *   - kernel_clone (5.x+)
 *   - _do_fork (4.x)
 *   - do_fork (3.x and earlier)
 */
static int fork_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    const struct cred *cred = current_cred();
    
    pr_info("PROC_MON [FORK] uid=%d pid=%d ppid=%d comm=%s\n",
            cred->uid.val,
            current->pid,
            current->real_parent->pid,
            current->comm);
    
    return 0;
}

/*
 * Module initialization
 * 
 * Sets up kprobes on execve and fork-related functions.
 * Kprobes allow us to intercept kernel function calls without
 * modifying the kernel source or using less stable methods
 * like syscall table hooking.
 */
static int __init procmon_init(void)
{
    int ret;
    
    pr_info("PROC_MON: Initializing process monitor\n");
    
    /* Hook execve - catches all program execution */
    kp_execve.symbol_name = "do_execveat_common";
    kp_execve.pre_handler = execve_pre_handler;
    
    ret = register_kprobe(&kp_execve);
    if (ret < 0) {
        /* Try alternative symbol for older kernels */
        kp_execve.symbol_name = "do_execve";
        ret = register_kprobe(&kp_execve);
        if (ret < 0) {
            pr_err("PROC_MON: Failed to register execve kprobe: %d\n", ret);
            return ret;
        }
    }
    pr_info("PROC_MON: Registered kprobe at %s\n", kp_execve.symbol_name);
    
    /* Hook fork/clone - catches all process creation */
    kp_fork.symbol_name = "kernel_clone";
    kp_fork.pre_handler = fork_pre_handler;
    
    ret = register_kprobe(&kp_fork);
    if (ret < 0) {
        /* Try alternative for older kernels */
        kp_fork.symbol_name = "_do_fork";
        ret = register_kprobe(&kp_fork);
        if (ret < 0) {
            pr_warn("PROC_MON: Fork kprobe failed (non-fatal): %d\n", ret);
            /* Continue anyway - execve monitoring still works */
        }
    }
    
    if (ret == 0)
        pr_info("PROC_MON: Registered kprobe at %s\n", kp_fork.symbol_name);
    
    pr_info("PROC_MON: Module loaded successfully\n");
    return 0;
}

/*
 * Module cleanup
 * 
 * Unregisters kprobes. Always called when module is unloaded.
 * Failing to unregister would leave hooks in place and eventually
 * cause a crash when the hook tries to call into freed memory.
 */
static void __exit procmon_exit(void)
{
    unregister_kprobe(&kp_execve);
    unregister_kprobe(&kp_fork);
    
    pr_info("PROC_MON: Module unloaded\n");
}

module_init(procmon_init);
module_exit(procmon_exit);

