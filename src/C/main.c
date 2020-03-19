
#include "Drivers/interrupts.h"
#include "Drivers/timer.h"
#include "Drivers/uart.h"
#include "Drivers/emmc.h"

#include "Memory/kmalloc.h"
#include "Memory/memory_descriptor.h"
#include "Memory/mm.h"

#include "Processes/process.h"

#include "Libraries/printx.h"

extern void create_identity_map();

/* Use C linkage. */
#if defined(__cplusplus)
    extern "C" void test_process(char *str);
    extern "C" void kernel_main();
#endif

void test_process(char *str) __attribute__((__section__(".user")));

void test_process(char *str) {
    int x = 0;
    while (1) {
        int y = 0;
        for (int i = 0; i < 100000; i++) {
            y += i;
        }
        x++;
    }
}

void kernel_main() {
    init_uart();
    init_printf(0, putc);
    printf("Kernel Program Started.\n");
    init_emmc();

    enable_interrupt_controller();
    init_arm_timer(3000000);
    init_memory();

    init_process_manager();

    create_process(test_process, 0xDEAD10CC);
    create_process(test_process, 0xFFFFFFFFFF);

    enable_irq();

    int x = 0;
    while (1) {
        int y = 0;
        for (int i = 0; i < 100000; i++) {
            y += i;
        }
        x++;
    }
}