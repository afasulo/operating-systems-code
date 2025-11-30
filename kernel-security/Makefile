# Makefile for kernel security modules
#
# Usage:
#   make              - Build all modules
#   make clean        - Remove build artifacts
#   make install      - Load modules (requires root)
#   make uninstall    - Unload modules (requires root)
#
# Prerequisites:
#   sudo apt install linux-headers-$(uname -r) build-essential

obj-m += process_monitor.o
obj-m += syscall_integrity.o

# Get the running kernel's build directory
KDIR ?= /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

# Install modules (load into running kernel)
install: all
	@echo "Loading process_monitor..."
	sudo insmod process_monitor.ko
	@echo "Loading syscall_integrity..."
	sudo insmod syscall_integrity.ko
	@echo "Modules loaded. Check dmesg for output."

# Uninstall modules
uninstall:
	@echo "Unloading modules..."
	-sudo rmmod syscall_integrity 2>/dev/null
	-sudo rmmod process_monitor 2>/dev/null
	@echo "Modules unloaded."

# Show kernel log
log:
	sudo dmesg | tail -50 | grep -E "(PROC_MON|SYSCALL_CHK)"

.PHONY: all clean install uninstall log

