#include "Memory/mmu.h"
#include "Memory/memory_descriptor.h"

void create_TTBR0_tables() {
    /* TTBR0_Level2 -> 2MB BLocks of Physical Memory */
    int base = 0;
    for (; base < RAM_MAX; base++) {
        T0_L2[base].type = Block;
        T0_L2[base].address = base << Block_Shift_2MB;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Normal;
        T0_L2[base].SH = Inner;
    }

    for (; base < VRAM_MAX; base++) {
        T0_L2[base].type = Block;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Normal_nC;
        T0_L2[base].AP = AP_RW_RW;
        T0_L2[base].address = base << Block_Shift_2MB;
    }

    for (; base < 512; base++) {
        T0_L2[base].type = Block;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Device_nGnRE;
        T0_L2[base].AP = AP_RW_RW;
        T0_L2[base].address = base << Block_Shift_2MB;
    }

    /* Mailbox at 1024MB to 1026MB */
    T0_L2[base].type = Block;
    T0_L2[base].address = 512 << Block_Shift_2MB;
    T0_L2[base].AF = 1;
    T0_L2[base].memory_attributes = Device_nGnRnE;
    T0_L2[base].AP = AP_RW_RW;

    // base++;

    // /* Set remaining entries to empty */
    // for (; base < 1024; base++) {
    //     T0_L2[base] = (Block_Descriptor){0};
    // }

    /* TTBR0_Level1 -> TTBR0_Level2 */
    T0_L1[0].type = Table;
    T0_L1[0].APTable = 0;
    T0_L1[0].address = (uintptr_t)(&T0_L2[0]) >> Table_Shift_4kB;

    T0_L1[1].type = Table;
    T0_L1[1].APTable = 0;
    T0_L1[1].address = (uintptr_t)(&T0_L2[512]) >> Table_Shift_4kB;
    // T0_L1[0].NS = 1;

    //display_table(&T0_L1[0]);
}

void create_TTBR1_tables() {
    int base = 0;

    for (; base < 512; base++) {
        T1_L3[base] = (Block_Descriptor){0};
        T1_L2[base] = (Table_Descriptor){0};
        T1_L1[base] = (Table_Descriptor){0};
    }

    for (base = 0; base < 512; base++) {
        T1_L3[base].type = Page;
        T1_L3[base].address = base;
        T1_L3[base].AF = 1;
        T1_L3[base].memory_attributes = Normal;
        T1_L3[base].SH = Inner;
        T1_L3[base].AP = 1;
    }

    T1_L2[0].type = Table;
    T1_L2[0].address = (uintptr_t)(T1_L3) >> Table_Shift_4kB;
    T1_L2[0].NSTable = 1;
    T1_L2[0].APTable = APTable_RW_RW;

    T1_L1[0].type = Table;
    T1_L1[0].address = (uintptr_t)(T1_L2) >> Table_Shift_4kB;
    T1_L1[0].NSTable = 1;
    T1_L1[0].APTable = APTable_RW_RW;

    //display_table(T1_L1);
}

void init_mmu() {
    TCR_EL1 translation_control;
    SCTLR_EL1 system_control;

    create_TTBR0_tables();
    create_TTBR1_tables();

    asm volatile("dsb sy");
    int r = ((0x00ul << (0 * 8)) | (0x04ul << (1 * 8)) | (0x0cul << (2 * 8)) |
             (0x44ul << (3 * 8)) | (0xfful << (4 * 8)));
    asm volatile("msr mair_el1, %0" : : "r"(r));

    /* Enable the MMU */
    /* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.den0024a/CIHCHDIF.html
     */

    translation_control.T0SZ = T0SZ_512;
    translation_control.T1SZ = T1SZ_512;
    translation_control.EPD0 = 0;
    translation_control.EPD1 = 0;
    translation_control.IRGN0 = Write_Back_Cacheable;
    translation_control.IRGN1 = Write_Back_Cacheable;
    translation_control.ORGN0 = Write_Back_Cacheable;
    translation_control.ORGN1 = Write_Back_Cacheable;
    translation_control.SH0 = Inner;
    translation_control.SH1 = Inner;
    translation_control.TG0 = TG0_4;
    translation_control.TG1 = TG1_4;
    translation_control.TBI0 = 0;
    translation_control.TBI1 = 0;
    translation_control.IPS = IPS_32;

    asm volatile("msr ttbr0_el1, %0" ::"r"(&T0_L1));
    asm volatile("msr ttbr1_el1, %0" ::"r"(&T0_L1));
    asm volatile("msr tcr_el1, %0" ::"r"(translation_control));
    asm volatile("isb");

    asm volatile("mrs %0, sctlr_el1" : "=r"(system_control));
    system_control.MandatoryReserved1Bits |= 0xC00800;
    system_control.enable_MMU = 1;
    system_control.enable_alignment_fault_check = 1;
    system_control.enable_stack_alignment_check = 1;
    system_control.enable_stack_el0_alignment_check = 1;
    system_control.enable_data_cache = 1;
    system_control.enable_instruction_cache = 1;
    asm volatile("msr sctlr_el1, %0" ::"r"(system_control));
    asm volatile("isb");
}