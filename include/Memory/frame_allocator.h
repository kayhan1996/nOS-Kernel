#include <stdint.h>
#include "mm.h"

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#define MAX_ORDER   				11
#define KERNEL_ADDRESS(addr)        ((uint64_t)(addr) + 0xFFFFFF8000000000)
#define PHYSICAL_ADDRESS(addr)      ((uint64_t)(addr) - 0xFFFFFF8000000000)

struct LinkedList buckets[MAX_ORDER];

void init_buddy();
Frame * allocate_page_frames(uint64_t order);
void free_page_frames(Frame *frame);

#endif