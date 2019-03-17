#include "mmu.h"

#define BLOCK_SIZE          2
#define PERIPHERALS         16
#define VRAM                32
#define RAM                 1024

#define PERIPHERALS_MAX     (RAM / BLOCK_SIZE)
#define VRAM_MAX            (PERIPHERALS_MAX - (PERIPHERALS / BLOCK_SIZE))
#define RAM_MAX             (VRAM_MAX - (VRAM / BLOCK_SIZE))

#define LEVEL2_ADDRESS       (21-12)     //access bits [12:20] since 512 Entries

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
        T0_L2[base].address = base << LEVEL2_ADDRESS;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Normal;
        T0_L2[base].SH = Inner;
    }

    for(; base < VRAM_MAX; base++){
        T0_L2[base].type = Block;
        T0_L2[base].address = base << LEVEL2_ADDRESS;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Normal_nC;
    }

    for(; base <= PERIPHERALS_MAX; base++){
        T0_L2[base].type = Block;
        T0_L2[base].address = base << LEVEL2_ADDRESS;
        T0_L2[base].AF = 1;
        T0_L2[base].memory_attributes = Device_nGnRE;
    }
        /* Mailbox at 1024MB to 1026MB */
        T0_L2[512].type = Block;
        T0_L2[512].address = base << LEVEL2_ADDRESS;
        T0_L2[512].AF = 1;
        T0_L2[512].memory_attributes = Device_nGnRnE;

    /* Set remaining entries to empty */
    for(512; base < 1024; base++){
        T0_L2[base] = (Block_Descriptor) {0};
    }

    /* TTBR0_Level1 -> TTBR0_Level2 */
    T0_L1[0].type = Table;
    T0_L1[0].address = (uintptr_t)(&T0_L2[0]) >> 12;
    T0_L1[0].NS = 1;

    T0_L1[1].type = Table;
    T0_L1[1].address = (uintptr_t)(&T0_L2[512]) >> 12;
    T0_L1[1].NS = 1;
    
}

void create_TTBR1_tables(){
    for(int i = 0; i < 512; i++){
        T1_L1[i] = (Table_Desciptor){0};
        T1_L2[i] = (Table_Desciptor){0};
        T1_L3[i] = (Block_Descriptor){0};
    }

    /* TTBR1_L1 -> TTBR1_L2 */
    T1_L1[0].type = Table;
    T1_L1[0].address = (uintptr_t)(&T1_L2[0]) >> 12;
    T1_L1[0].NS = 1;

    /* TTBR1_l2 -> TTBR1_L3 */
    T1_L2[1].type = Table;
    T1_L2[1].address = (uintptr_t)(&T1_L3[512]) >> 12;
    T1_L2[1].NS = 1;
}

void init_mmu(){
    TCR_EL1 translation_control;
    SCTLR_EL1 system_control;

    create_TTBR0_tables();
    create_TTBR1_tables();

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

uint64_t virtualmap (uint32_t phys_addr) {
	uint64_t addr = 0;

		// if (T0_L2[513].data == 0) {							// Find the first vacant stage3 table slot
		// 	uint64_t offset;

		// 	T0_L2[513] = (Block_Descriptor) { 
        //         .address = (uintptr_t)phys_addr << (21 - 12),
        //         .AF = 1,
        //         .memory_attributes = Normal,
        //         .type = Block 
        //     };

		// 	asm volatile ("dmb sy" ::: "memory");
		// 	offset = ((512 - i) * 4096) - 1;
		// 	addr = 0xFFFFFFFFFFFFFFFFul;
		// 	addr = addr - offset;
		// 	return(addr);
		// }

	return (addr);													// error
}