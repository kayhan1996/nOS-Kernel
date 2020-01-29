#ifndef __PAGE_TABLE__
#define __PAGE_TABLE__

#include "Memory/memory_descriptor.h"
#include "Memory/mmu.h"

typedef Table_Desciptor Page_Table;

void create_new_table(Memory_Descriptor* mem);
void free_table(Page_Table* table);
void switch_virtual_map(Page_Table* table);

#endif /* __PAGE_TABLE__ */