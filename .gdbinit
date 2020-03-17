file build/bin/kernel8.elf
target remote 192.168.1.141:3333
monitor reset halt
shell sleep 1
set remote hardware-breakpoint-limit 6
set remote hardware-watchpoint-limit 4
load
monitor step 0x80000
stepi
hbreak kernel_main
continue
shell sleep 1
clear
#delete 0xffffff8000083834
#file build/kernel.sym
#tui enable
