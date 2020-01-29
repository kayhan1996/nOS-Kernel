#include "Memory/kmalloc.h"
#include "Libraries/linked_list.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct memory_header {
    LinkNode node;
    int size;
    bool free;
    uint32_t magic;
} Memory_Header;

LinkedList free_memory;

Memory_Header *find_free_memory(uint32_t size) {
    Memory_Header *current = payload(Memory_Header, free_memory.head);

    while (current != NULL && current->node.next != NULL &&
           current->size < size) {
        current = payload(Memory_Header, current->node.next);
    }

    if (current == NULL || current->size < size) {
        return NULL;
    }

    return free_memory.remove_end(&free_memory);
}

void init_kmalloc() { init_linked_list(&free_memory); }

int calculate_min_order(int size) { return size >> 12; }

Memory_Header *allocate_memory(uint32_t size) {
    printf("size of memory header: %d\n", sizeof(Memory_Header));
    int t = calculate_min_order(size + sizeof(Memory_Header));
    Frame *frame = allocate_page_frames(t);
    t = (1 << t) << 12; // 4096*(2^t)

    printf("address: %x\n", frame->address + size + sizeof(Memory_Header));

    Memory_Header *header =
        (Memory_Header *)(frame->address + sizeof(Memory_Header) + size);
    header->size = t - size;
    header->free = true;
    header->magic = 0xC0FFEE;
    free_memory.add(&free_memory, &header->node);

    header = (Memory_Header *)frame->address;
    header->size = size;
    header->free = false;
    header->magic = 0xC0FFEE;
    return header;
}

int merge_memory(Memory_Header *header) {

    /* Inefficient, improve using AVL tree instead of linked list? */

    free_memory.add_end(&free_memory, &header->node);
    return 0;
}

void *kmalloc(uint32_t size, uint32_t flags) {
    Memory_Header *free_mem;

    if ((free_mem = find_free_memory(size)) == NULL) {
        free_mem = allocate_memory(size);
    } else {
        Memory_Header *header =
            (Memory_Header *)((void *)free_mem + sizeof(Memory_Header) + size);
        header->size = free_mem->size - size;
        header->free = true;
        header->magic = 0xC0FFEE;
        free_memory.add(&free_memory, &header->node);

        free_mem->size = size;
        free_mem->free = false;
        free_mem->magic = 0xC0FFEE;
    }

    return (void *)((long)free_mem + sizeof(Memory_Header));
}

int kfree(void *address) {
    address -= sizeof(Memory_Header);
    Memory_Header *header = (Memory_Header *)address;
    if (header->magic == 0xC0FFEE) {
        header->free = true;
        merge_memory(header);
        return 0;
    }
}