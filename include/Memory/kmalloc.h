#ifndef __KMALLOC__
#define __KMALLOC__

#include "Memory/frame_allocator.h"
#include "stdint.h"

void init_kmalloc();
void *kmalloc(uint32_t size, uint32_t flags);
int kfree(void *address);

#endif /* __KMALLOC__ */