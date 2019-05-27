#include "mm.h"
#include "stdint.h"

#define MEMORY_SIZE             (1024*1024*1024)
#define PAGE_SIZE               (4*1024)
#define PAGES_N                 (MEMORY_SIZE/PAGE_SIZE)

#define KERNEL_SIZE             (8*1024*1024)
#define KERNEL_PAGES_N          (KERNEL_SIZE/PAGE_SIZE)


struct mmap {
    uint64_t kernel    : 1;
    uint64_t allocated : 1;
    uint64_t reserved  : 62;
};

struct mmap memory_map[PAGES_N];

void init_memory_manager(){
    int i;

    for(i = 0; i < KERNEL_PAGES_N; i++){
        memory_map[i].allocated = 1;
        memory_map[i].kernel = 1;
    }

    for(; i < PAGES_N; i++){
        memory_map[i].allocated = 0;
        memory_map[i].kernel = 0;
    }
}

void *allocate_page_frame(){
    static uint64_t i = KERNEL_PAGES_N;
    for(; i < PAGES_N; i++){
        if(memory_map[i].allocated == 0){
            return (void *)(i * PAGE_SIZE);
        }
    }
    i = KERNEL_PAGES_N;
    return -1;
}