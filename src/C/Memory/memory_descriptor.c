#include "Memory/memory_descriptor.h"
#include "Libraries/utils.h"
#include "Memory/kmalloc.h"
#include "Memory/mmu.h"

typedef struct table_offsets {
    uint64_t level1;
    uint64_t level2;
    uint32_t level3;
} Table_Offsets;

/* Returns the virtual memory table positions of an address */
struct table_offsets get_table_offsets(uint64_t address) {
    struct table_offsets offsets;
    offsets.level1 = GET_BITS(address, 38, 30);
    offsets.level2 = GET_BITS(address, 29, 21);
    offsets.level3 = GET_BITS(address, 20, 12);

    return offsets;
}

Memory_Descriptor create_process_memory_descriptor() {
    Memory_Descriptor md;

    md.stack.start = (100*1024*1024);
    md.stack.size = 2 * 4096;
    md.stack.end = md.stack.start + md.stack.size;

    md.heap.start = (50*1024*1024);
    md.heap.size = 0;
    md.heap.end = md.heap.start + md.heap.size;

    md.text.start = 0x2000000;
    md.text.size = (8 * 4096);
    md.text.end = md.text.start + md.text.size;

    return md;
}

Table_Descriptor init_table_table(Table_Descriptor tables[]) {
    Table_Descriptor table = {0};
    table.type = Table;
    table.address = (uintptr_t)tables >> 12;
    table.NSTable = 1;
    table.APTable = APTable_RW_RW;
    return table;
}

Table_Descriptor init_block_table(Block_Descriptor blocks[]) {
    Table_Descriptor table = {0};
    table.type = Table;
    table.address = (uintptr_t)blocks >> 12;
    table.NSTable = 1;
    table.APTable = APTable_RW_RW;
    return table;
}

Block_Descriptor init_block() {
    Block_Descriptor block = {0};

    /* Allocate Memory and set block descriptor
       to point to that memory */
    void* MemoryBlock = allocate_page_frames(0)->address;
    block.type = Page;
    block.address = (uintptr_t)MemoryBlock >> 12;
    block.AF = 1;
    block.memory_attributes = Normal;
    block.SH = Inner;
    block.AP = AP_RW_RW;

    return block;
}

void display_table(Table_Descriptor *table) {

    for (int i = 0; i < 512; i++) {

        if (table[i].address != 0) {

            uint64_t addr = table[i].address << 12;
            printf("Table[%d](@0x%x):\n", i, addr);

            Table_Descriptor *L2 = (Table_Descriptor *)addr;

            for (int j = 0; j < 512; j++) {

                if (L2[j].type == Block){
                    uint64_t addr = L2[j].address << 12;
                    printf("    L2[%d]--BLOCK-->0x%x\n", j, addr);
                    continue;
                }

                if (L2[j].address != 0) {
                    uint64_t addr = L2[j].address << 12;
                    printf("    L2[%d](@0x%x):\n", j, addr);

                    Block_Descriptor *L3 = (Block_Descriptor *)addr;

                    for (int k = 0; k < 512; k++) {
                        uint64_t addr = L3[k].address << 12;
                        if (L3[k].address != 0) {
                            printf("        L3[%d](@0x%x):\n", k, addr);
                        }
                    }
                }
            }
        }
    }
}

void create_area(Table_Descriptor table[], Memory_Area *area) {
    Table_Descriptor *L2;
    Block_Descriptor *L3;

    Table_Offsets offset = get_table_offsets(area->start);
    int i = offset.level1;
    int j = offset.level2;
    int k = offset.level3;

    offset = get_table_offsets(area->end);
    int j_end = offset.level2;
    int k_end = offset.level3;

    int k_last = 511;

    if(table[i].data == 0)
        table[i] = init_table_table(L2 = allocate_page_frames(0)->address);
    else
        L2 = table[i].address << 12;

    while (j <= j_end) {
        if (j == j_end)
            k_last = k_end;

        if(L2[j].data == 0)
            L2[j] = init_block_table(L3 = allocate_page_frames(0)->address);
        else
            L3 = L2[j].address << 12;

        while (k < k_last) {
            L3[k] = init_block();
            k++;
        }

        k = 0;
        j++;
    }
}

Table_Descriptor *create_new_table(Memory_Descriptor *mem) {
    /* Level 1 Table */
    Table_Descriptor *Table = allocate_page_frames(0)->address;

    /* Create tables for each memory area */
    create_area(Table, &mem->stack);
    create_area(Table, &mem->heap);
    create_area(Table, &mem->text);

    display_table(Table);

    /* Set virtual memory User space tables */
    asm volatile("msr ttbr1_el1, %0" ::"r"(Table));
    asm volatile("isb");
    asm volatile("dmb sy" ::: "memory");
}