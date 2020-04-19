
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

#include "Filesystem/FAT32/ff.h"

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

int strlen(char *s){
    int count = 0;
    while(*s++ != '\0')
        count++;
    return count;
}

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}

void kernel_main() {
    init_uart();
    init_printf(0, putc);
    printf("\033[2J\033[H");
    printf("Kernel Program Started.\n");

    u64 speed_mhz = get_arm_clock() / 1e6;
    printf("Clock Speed: %u MHz\n", speed_mhz);

    FATFS fs;
    f_mount(&fs, "", 0);

    char line[100];
    FIL file;
    f_open(&file, "config.txt", FA_READ);
    while(f_gets(&line, sizeof(line), &file)){
        printf(line);
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