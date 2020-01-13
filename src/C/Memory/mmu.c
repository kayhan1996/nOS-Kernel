#include "Memory/mmu.h"

#define BLOCK_SIZE          2
#define PERIPHERALS         16
#define VRAM                32
#define RAM                 1024

#define PERIPHERALS_MAX     (RAM / BLOCK_SIZE)
#define VRAM_MAX            (PERIPHERALS_MAX - (PERIPHERALS / BLOCK_SIZE))
#define RAM_MAX             (VRAM_MAX - (VRAM / BLOCK_SIZE))

#define Table_Shift_4kB         12    //since tables are aligned 2^12 = 4096 bytes
#define Table_Shift_2MB         9     // (2^12)(2^9) = 2MB
#define Table_shift_1GB         9     // (2^12)(2^9)(2^9) = 1GB
#define Block_Shift_4kB         12    //since blocks are aligned 2^12 = 4096 bytes
#define Block_Shift_2MB         9     //since blocks are 2MB aligned so (2^12) x (2^9) = 2MB


static __attribute__((aligned(4096))) Block_Descriptor T0_L2[512];
static __attribute__((aligned(4096))) Table_Desciptor  T0_L1[512];

static __attribute__((aligned(4096))) Block_Descriptor T1_L3[512];
static __attribute__((aligned(4096))) Table_Desciptor  T1_L2[512];
static __attribute__((aligned(4096))) Table_Desciptor  T1_L1[512];


void create_TTBR0_tables(){

    /* TTBR0_Level2 -> 2MB BLocks of Physical Memory */
    int base = 0;
    for(; base < RAM_MAX; base++){
        T0_L2[base].type = Block;
        T0_L2[base].address = base << Block_Shift_2MB;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Normal;
        T0_L2[base].SH = Inner;        
    }

    for(; base < VRAM_MAX; base++){
        T0_L2[base].type = Block;
        T0_L2[base].address = base << Block_Shift_2MB;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Normal_nC;
    }

    for(; base <= PERIPHERALS_MAX; base++){
        T0_L2[base].type = Block;
        T0_L2[base].address = base << Block_Shift_2MB;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Device_nGnRE;
    }
        /* Mailbox at 1024MB to 1026MB */
        T0_L2[512].type = Block;
        T0_L2[512].address = base << Block_Shift_2MB;
        T0_L2[512].AF = 1;
        T0_L2[512].memory_attributes = Device_nGnRnE;



    /* Set remaining entries to empty */
    //To test MMU is working, move all entries above 1026 to point to 0x0000
    for(; base < 1024; base++){
        if(base > 512){
            T0_L2[base] = (Block_Descriptor) {
                .type = Block,
                .address = 0 << Block_Shift_2MB,
                .AF = 1,
                .memory_attributes = Normal,
                .SH = Inner
            };
        }
        //T0_L2[base] = (Block_Descriptor) {0};
    }

    /* TTBR0_Level1 -> TTBR0_Level2 */
    T0_L1[0].type = Table;
    T0_L1[0].address = (uintptr_t)(T0_L2) >> Table_Shift_4kB;
    T0_L1[0].NS = 1;    
}

void create_TTBR1_tables(){
    int base = 0;

    for(; base < 512; base++){
        T1_L3[base] = (Block_Descriptor){0};
        T1_L2[base] = (Table_Desciptor){0};
        T1_L1[base] = (Table_Desciptor){0};
    }

    for(base = 0; base < 512; base++){
        T1_L3[base].type = Page;
        T1_L3[base].address = base;
        T1_L3[base].AF = 1;
        T1_L3[base].memory_attributes = Normal;
        T1_L3[base].SH = Inner;
    }

    T1_L2[0].type = Table;
    T1_L2[0].address = (uintptr_t)(T1_L3) >> Table_Shift_4kB;
    T1_L2[0].NS = 1;

    T1_L1[0].type = Table;
    T1_L1[0].address = (uintptr_t)(T1_L2) >> Table_Shift_4kB;
    T1_L1[0].NS = 1;    
}

void init_mmu(){
    TCR_EL1 translation_control;
    SCTLR_EL1 system_control;

    create_TTBR0_tables();
    //create_TTBR1_tables();

    asm volatile("dsb sy");
	int r = ((0x00ul << (0 * 8)) | \
		 (0x04ul << (1 * 8)) | \
		 (0x0cul << (2 * 8)) | \
		 (0x44ul << (3 * 8)) | \
		 (0xfful << (4 * 8)));
    asm volatile ("msr mair_el1, %0" : : "r" (r));

    /* Enable the MMU */
    /* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.den0024a/CIHCHDIF.html */

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

    asm volatile("msr ttbr0_el1, %0" :: "r" (&T0_L1));
    asm volatile("msr ttbr1_el1, %0" :: "r" (&T1_L1));
    asm volatile("msr tcr_el1, %0" :: "r" (translation_control));
    asm volatile("isb");

    asm volatile("mrs %0, sctlr_el1" : "=r"(system_control));
    system_control.MandatoryReserved1Bits |= 0xC00800;
    system_control.enable_MMU = 1;
    system_control.enable_alignment_fault_check = 1;
    system_control.enable_stack_alignment_check = 1;
    system_control.enable_stack_el0_alignment_check = 1;
    system_control.enable_data_cache = 1;
    system_control.enable_instruction_cache = 1;
    asm volatile("msr sctlr_el1, %0" :: "r" (system_control));
    asm volatile("isb");
}

uint64_t* map_physical_to_virtual(uintptr_t address){
    uint64_t *vaddr = 0;
    int i = 0;
    for(; i < 512; i++){
        if(T1_L3[i].data == 0){
            T1_L3[i].type = 3;
            T1_L3[i].address = address << Block_Shift_4kB;
            T1_L3[i].AF = 1;
            T1_L3[i].memory_attributes = Normal;
            T1_L3[i].SH = Inner;
            break;
        }
    }

    uint64_t base_address = 0xFFFFFF8000000000;             //base address of TTBR1
    vaddr = base_address;                     //offset to correct page table
    asm volatile ("dmb sy" ::: "memory");

    return vaddr;
}