
#include "Drivers/interrupts.h"
#include "Drivers/timer.h"
#include "Drivers/uart.h"
#include "Libraries/printx.h"
#include "Memory/mm.h"
#include "Processes/process.h"

#include "Memory/kmalloc.h"
#include "Memory/memory_descriptor.h"

/* Use C linkage. */
#if defined(__cplusplus)
extern "C" void test_process(char *str);
extern "C" void kernel_main();
#endif

void test_process(char *str) __attribute__((__section__(".USER")));

void test_process(char *str) {
    int x = 0;
    while (1) {
        int y = 0;
        for (int i = 0; i < 100000; i++) {
            y += i;
        }
        x++;
        printf(str);
        printf("[%d]\n", x);
    }
}

void kernel_main() {
    init_uart();
    init_printf(0, putc);
    printf("Kernel Program Started.\n");

    enable_interrupt_controller();
    init_arm_timer(3000000);
    enable_irq();

    init_memory();

    init_process_manager();

    printf("Hadoop!\n");

    //create_process(test_process, "abcde");
    //create_process(test_process, "     		 12345");

    int x = 0;
    while (1) {
        int y = 0;
        for (int i = 0; i < 100000; i++) {
            y += i;
        }
        x++;
        printf("                                      Kernel[%d]\n", x);
    }
}