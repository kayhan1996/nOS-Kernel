#include "mm.h"
#include "process.h"
#include "printx.h"

#define MAX(a,b)                (((a)>(b))?(a):(b))

#define MEMORY                  (1024 * 1024 * 1024)
#define PAGE_SIZE               (4096)
#define PAGES_N                 (MEMORY/PAGE_SIZE)

#define KERNEL_SIZE             (8 * 1024 * 1024)
#define KERNEL_PAGES_N          (KERNEL_SIZE/PAGE_SIZE)

#define PAGE_TABLE_SIZE         (PAGES_N * sizeof(struct PageFrame))
#define PAGE_TABLE_N            ((PAGE_TABLE_SIZE/PAGE_SIZE) + KERNEL_PAGES_N)
#define PAGE_TABLE_BASE         (KERNEL_SIZE)

#define PROCESS_TABLE_SIZE      4096
#define PROCESS_TABLE_N         ((PROCESS_TABLE_SIZE/PAGE_SIZE) + PAGE_TABLE_N)
#define PROCESS_TABLE_BASE      (KERNEL_SIZE + PAGE_TABLE_SIZE)

struct PageFrame *PageTable;

void init_memory_manager(){
    PageTable = (struct PageFrame*) PAGE_TABLE_BASE;
    int i = 0;

    for(; i < KERNEL_PAGES_N; i++){
        PageTable[i].allocated = 1;
        PageTable[i].kernel = 1;
    }

    for(; i < PAGE_TABLE_N; i++){
        PageTable[i].allocated = 1;
        PageTable[i].kernel = 1;
    }

    for(; i < PROCESS_TABLE_N; i++){
        PageTable[i].allocated = 1;
        PageTable[i].kernel = 1;
    }

    for(; i < PAGES_N; i++){
        PageTable[i].allocated = 0;
        PageTable[i].kernel = 0;
    }

    printf("Page Table Base: 0x%x\n", PAGE_TABLE_BASE);
    printf("Page Table Size: 0x%d\n", PAGE_TABLE_SIZE);
    printf("Process Table Base: 0x%x\n", PROCESS_TABLE_BASE);
    printf("Porcess Table Size: 0x%d\n", PROCESS_TABLE_SIZE);
    
}

void *allocate_page_frame(){
    for(int i = 0; i < PAGES_N; i++){
        if(PageTable[i].allocated == 0){
            PageTable[i].allocated = 1;
            PageTable[i].kernel = 0;
            return (i * PAGE_SIZE);
        }
    }
    return -1;
}

void deallocate_page_frame(void *frame){
    uint64_t i = (uint64_t)frame / PAGE_SIZE;
    PageTable[i].allocated = 0;
}