#include <stdint.h>

#include "Memory/mm.h"
#include "Memory/frame_allocator.h"

#define NOT_PAGE_HEAD								(15)

/* Adds a frame of the given order to the bucket list */
static inline void add_frame(Frame *frame, int order){
	buckets[order].add(&buckets[order], &frame->node);
}

/* Retrieves a frame of the given order from the front of the bucket list */
static inline Frame * get_frame(int order){
	Frame *frame = payload(Frame, buckets[order].remove(&buckets[order]));
	return frame;
}

/* Removes the frame from the bucket list */
static inline void remove_frames(Frame *frame){
	buckets[frame->order].remove_node(&buckets[frame->order], &frame->node);
}

/* Merges two frames and returns the resulting frame */
/* Requirement: buddy must be in a bucket list */
static inline Frame * merge_frames(Frame *frame, Frame *buddy){
		remove_frames(buddy);
		if(buddy > frame){
			buddy->order = NOT_PAGE_HEAD;
			frame->order += 1;
			return frame;
		}else{
			buddy->order += 1;
			frame->order = NOT_PAGE_HEAD;
			return buddy;
		}
}

/* Finds the corresponding buddy frame to the given frame */
static inline Frame * find_buddy(Frame *frame){
	uint64_t i = ((uint64_t)frame->address ^ (1 << (frame->order+12))) >> 12;
	return &AllPages[i];
	
}

/* Splits a given frame in half, and adds the buddy to the correct list */
static inline void split_frame(Frame *frame){
	Frame *buddy = frame + (1 << (--frame->order));
	add_frame(buddy, buddy->order = frame->order);
	//printf("    >Buddy frame %ld\n", buddy->address/PAGE_SIZE);	
}

/* Allocates page frames of the given order, and returns the first frame */
/* Requirement: order < MAX_ORDER */
Frame * allocate_page_frames(uint64_t order){
	int next_order = order;

	while(buckets[next_order].count == 0) next_order++;

	Frame *frame = get_frame(next_order);

	while(next_order-- != order) split_frame(frame);
	
	frame->allocated = 1;

    frame->address += 0xFFFFFF8000000000;

	uint64_t *start = frame->address;

	for(int i = 0; i < 512; i++){
		start[i] = 0x0;
	}

    //printf("Allocate frame %ld\n", (long)frame->address/PAGE_SIZE);

    return frame;
}

void free_page_frames(Frame *frame){
	//printf("Freeing %ld\n", frame->address/PAGE_SIZE);
	frame->allocated = 0;
	Frame *buddy = find_buddy(frame);
	while(frame->order < MAX_ORDER-1 && frame->order == buddy->order && frame->allocated == 0 && buddy->allocated == 0){
		//printf("    Merging %ld & %ld\n", frame->address/PAGE_SIZE, buddy->address/PAGE_SIZE);
		frame = merge_frames(frame, buddy);
		buddy = find_buddy(frame);
	}
	add_frame(frame, frame->order);	
}

void init_buddy(){
	//Zero all buckets
	for(int i = 0; i < MAX_ORDER; i++)
		init_linked_list(&buckets[i]);

	/* Add pages of order 10 to the bucket list */
	for(int i = KERNEL_RESERVED; i < NUM_PAGES; i+=1024){
		AllPages[i].order = MAX_ORDER - 1;
		buckets[MAX_ORDER-1].add_end(&buckets[MAX_ORDER-1], &AllPages[i].node);
	}
}