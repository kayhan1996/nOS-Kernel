#include "Memory/mm.h"
#include "Memory/frame_allocator.h"
#include "Memory/kmalloc.h"
#include "Memory/mmu.h"


/* Finds the index of the frame in the AllPages array */
int find_index(Frame *frame){
	return (frame-AllPages);
}

extern uint32_t __end;

/* initializes the memory and buddy system */
void init_memory(){
	AllPages = &__end;
	int i = 0;
	uint64_t address = 0;

	for(; i < KERNEL_RESERVED; i++){
		AllPages[i] = (Frame){
			.node = {NULL, NULL},
			.address = address,
			.allocated = 0,
			.kernel = 1,
			.order = 15
		};
		address += PAGE_SIZE;
	}

	for(; i < NUM_PAGES; i++){
		AllPages[i] = (Frame){
			.node = {NULL, NULL},
			.address = address,
			.allocated = 0,
			.kernel = 0,
			.order = 15
		};
		address += PAGE_SIZE;
	}

	init_buddy();
  init_kmalloc();
  init_mmu();
}
