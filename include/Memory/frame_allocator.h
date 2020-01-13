#include <stdint.h>
#include "mm.h"

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#define MAX_ORDER				11

struct LinkedList buckets[MAX_ORDER];

void init_buddy();
Frame * allocate_page_frames(uint64_t order);
void free_page_frames(Frame *frame);

#endif