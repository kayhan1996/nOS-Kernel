
#include "Drivers/interrupts.h"
#include "Drivers/timer.h"
#include "Drivers/uart.h"
#include "Drivers/emmc.h"
#include "Drivers/mailbox.h"

#include "Memory/kmalloc.h"
#include "Memory/memory_descriptor.h"
#include "Memory/mm.h"

#include "Processes/process.h"

#include "Libraries/printx.h"
#include "Libraries/utils.h"

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

u32 get_arm_clock(){
    u32 message[8];
    message[0] = 8*sizeof(u32);

    message[1] = 0;
    message[2] = 0x30002;
    message[3] = 8;
    message[4] = 4;
    message[5] = 3;
    message[6] = 0;

    message[7] = 0;

    call_mailbox(message, 8);

    return message[6];
}

void kernel_main() {
    init_uart();
    init_printf(0, putc);
    printf("\033[2J\033[H");
    printf("Kernel Program Started.\n");

    u64 speed_mhz = get_arm_clock() / 1e6;
    printf("Clock Speed: %u MHz\n", speed_mhz);

    u32 bytes[128];
    for(int i = 0; i < 128; i++){
        bytes[i] = 0xFF;
    }

    init_emmc();

    printf("SD Card Read:\n");
for(int z = 0; z < 1; z++){
    read_emmc(z << 9, bytes);

    for(int i = 0; i < 128; i+=8){
        for(int j = 0; j < 8; j++){
            printf("%08x ", bytes[i+j]);
        }
        printf("\n");
    }
}
    
    enable_interrupt_controller();
    init_arm_timer(3000000);
    init_memory();

    init_process_manager();

    //create_process(test_process, 0xDEAD10CC);
    //create_process(test_process, 0xFFFFFFFFFF);

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