#ifndef __MM_H__
#define __MM_H__

#include <stdint.h>

struct PageFrame {
    uint64_t allocated  : 1;
    uint64_t kernel     : 1;
};

void init_memory_manager();
void *allocate_page_frame();
void deallocate_page_frame(void *frame);

#endif // __MM_H__
