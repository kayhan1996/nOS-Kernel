#include "Memory/memory_descriptor.h"
#include "Memory/kmalloc.h"

void create_process_memory(Memory_Descriptor *md){
    md->stack.end = 0x0000FFFFFFFFFFFF;
    md->stack.start = 0x0000FFFFFFFFFFFF;
    md->stack.size = 0;

    md->heap.end = 0xC800000‬;
    md->heap.start = 0xC800000‬;
    md->heap.size = 0;

    md->text.end = 0x1000000;
    md->text.start = 0x80000;
    md->text.size = md->text.end-md->text.start;
}