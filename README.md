# nOS Bare-Metal Operating System Kernel for Raspberry Pi 3B
Operating system kernel for Armv8 Raspberry Pi devices.

# Features
 - Stack-based process context switching
 - Improved security using EL0 exception level for user processes
 - Virtual Memory using the hardware MMU for process isolation
 - Kernel memory management based on the Buddy Allocator algorithm

# Building
Install the GNU aarch64-none-elf toolchain from https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads for your system, and either add the toolchain bin directory to your environment path, or edit the makefile to point to the precompiled binaries. Run `make build` to build the kernel file `kernel8.img`, which is located in the `build` folder, which can be loaded onto a SD card and used on the raspberry pi model 3B.

# Emulating on QEMU
Build and install the latest qemu for aarch64, `qemu-system-aarch64`, using the qemu sources or if you are using Ubuntu, you can get the prebuilt binary using `sudo apt install qemu-system-arm`. This kernel uses the generic ARM core system timers rather than Broadcom timer for process interrupts, which will allow it to run under older QEMU builds. The system can be run directly from the makefile using `make run`.
