#include <stdint.h>
#include <linked_list.h>

#ifndef __MM_H__
#define __MM_H__


#define MEMORY_SIZE             256
#define MEMORY_SIZE_BYTES       (MEMORY_SIZE * 1024 * 1024)

#define PAGE_SIZE               4096
#define NUM_PAGES               (MEMORY_SIZE_BYTES/PAGE_SIZE)

#define KERNEL_SIZE             16
#define KERNEL_SIZE_BYTES       (KERNEL_SIZE*1024*1024)
#define KERNEL_RESERVED         (KERNEL_SIZE_BYTES/PAGE_SIZE)

typedef struct frame {
	   	LinkNode node;
	    uint64_t address;
		uint64_t allocated : 1;
		uint64_t kernel : 1;
		uint64_t order : 4;
} Frame;


Frame *AllPages;
uint64_t *memory;

int find_index(struct frame *frame);
void init_memory();

#endif