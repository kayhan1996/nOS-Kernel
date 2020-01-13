#ifndef __MEMORY_DESCRIPTOR__
#define __MEMORY_DESCRIPTOR__

#include <stdint.h>
#include "Memory/mmu.h"

typedef struct memory_area {
    uint64_t start;
    uint64_t end;
    uint64_t size;
} Memory_Area;

typedef struct memory_descriptor {
    Memory_Area stack;
    Memory_Area heap;
    Memory_Area bss;
    Memory_Area data;
    Memory_Area text;
    Table_Desciptor page_table;
} Memory_Descriptor;

#endif /* __MEMORY_DESCRIPTOR__ */