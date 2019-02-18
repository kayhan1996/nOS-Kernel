#include "mmu.h"

static __attribute__((aligned(4096))) Block_Descriptor TTBR0_Level2[512*2];
static __attribute__((aligned(4096))) Table_Descriptor TTBR0_Level1[512];

static __attribute__((aligned(4096))) Block_Descriptor TTBR1_Level3[512*2];
static __attribute__((aligned(4096))) Table_Descriptor TTBR1_Level2[512];
static __attribute__((aligned(4096))) Table_Descriptor TTBR1_Level1[512];

void create_TTBR0_tables(){
    int base = 0;

    //First 880Mb dedicated t oRAM
    for(base = 0; base < 440; base++){
        TTBR0_Level2[base] = (Block_Descriptor) {
            .Address = (uintptr_t)base << (21-12),
            .AccessFlag = 1,
            .SH = INNER_SHAREABLE,
            .MemoryAttribute = NORMAL,
            .EntryType = 1  //block table
        };
    }

    //64MB dedicated to Video Ram
    for(; base < 512 - 8; base++){
        TTBR0_Level2[base] = (Block_Descriptor) {
            .Address = (uintptr_t)base << (21-12),
            .AccessFlag = 1,
            .MemoryAttribute = NORMAL_NC,
            .EntryType = 1  //block table
        };
    }

    //16MB reserved memory for Peripherals
    for(; base <= 512; base++){
        TTBR0_Level2[base] = (Block_Descriptor) {
            .Address = (uintptr_t)base << (21-12),
            .AccessFlag = 1,
            .MemoryAttribute = DEVICE_NGNRE,
            .EntryType = 1  //block table
        };
    }

    //2MB for mailbox
    TTBR0_Level2[512] = (Block_Descriptor) {
            .Address = (uintptr_t)base << (21-12),
            .AccessFlag = 1,
            .MemoryAttribute = DEVICE_NGNRNE,
            .EntryType = 1  //block table
    };

    //Zero rest of data
    for(base = 513; base <= 1024; base++){
        TTBR0_Level2[base].RawData = 0;
    }

    TTBR0_Level1[0] = (Table_Descriptor) {
        .NSTable = 1,
        .Address = (uintptr_t)&TTBR0_Level2[0] >> 12,
        .EntryType = 3
    };

    TTBR0_Level1[1] = (Table_Descriptor) {
        .NSTable = 1,
        .Address = (uintptr_t)&TTBR0_Level2[512] >> 12,
        .EntryType = 3
    };
}

void create_TTBR1_tables(){
    for(int i = 0; i < 512; i++){
        TTBR1_Level1[i].RawData = 0;
        TTBR1_Level2[i].RawData = 0;
        TTBR1_Level3[i].RawData = 0;
    }

    TTBR1_Level1[511] = (Table_Descriptor) {
        .NSTable = 1,
        .Address = (uintptr_t)&TTBR1_Level2[0] >> 12,
        .EntryType = 3
    };

    TTBR1_Level2[511] = (Table_Descriptor) {
        .NSTable = 1,
        .Address = (uintptr_t)&TTBR1_Level3[0] >> 12,
        .EntryType = 3
    };
}

void init_mmu(){
    create_TTBR0_tables();
    create_TTBR1_tables();

    uint64_t r;

    asm volatile("dsb sy");
    asm volatile("dsb sy");
	r = ((0x00ul << (MT_DEVICE_NGNRNE * 8)) | \
		 (0x04ul << (MT_DEVICE_NGNRE * 8)) | \
		 (0x0cul << (MT_DEVICE_GRE * 8)) | \
		 (0x44ul << (MT_NORMAL_NC * 8)) | \
		 (0xfful << (MT_NORMAL * 8)));
    asm volatile ("msr mair_el1, %0" : : "r" (r));

    // Bring both tables online and execute memory barrier
	asm volatile ("msr ttbr0_el1, %0" : : "r" ((uintptr_t)&TTBR0_Level1[0]));
	asm volatile ("msr ttbr1_el1, %0" : : "r" ((uintptr_t)&TTBR1_Level1[0]));
	asm volatile("isb");

    // Specify mapping characteristics in translate control register
    r = (0b00LL << 37) | // TBI=0, no tagging
        (0b000LL << 32)| // IPS= 32 bit ... 000 = 32bit, 001 = 36bit, 010 = 40bit
        (0b10LL << 30) | // TG1=4k ... options are 10=4KB, 01=16KB, 11=64KB ... take care differs from TG0
        (0b11LL << 28) | // SH1=3 inner ... options 00 = Non-shareable, 01 = INVALID, 10 = Outer Shareable, 11 = Inner Shareable
        (0b01LL << 26) | // ORGN1=1 write back .. options 00 = Non-cacheable, 01 = Write back cacheable, 10 = Write thru cacheable, 11 = Write Back Non-cacheable
        (0b01LL << 24) | // IRGN1=1 write back .. options 00 = Non-cacheable, 01 = Write back cacheable, 10 = Write thru cacheable, 11 = Write Back Non-cacheable
        (0b0LL  << 23) | // EPD1 ... Translation table walk disable for translations using TTBR1_EL1  0 = walk, 1 = generate fault
        (25LL   << 16) | // T1SZ=25 (512G) ... The region size is 2 POWER (64-T1SZ) bytes
        (0b00LL << 14) | // TG0=4k  ... options are 00=4KB, 01=64KB, 10=16KB,  ... take care differs from TG1
        (0b11LL << 12) | // SH0=3 inner ... .. options 00 = Non-shareable, 01 = INVALID, 10 = Outer Shareable, 11 = Inner Shareable
        (0b01LL << 10) | // ORGN0=1 write back .. options 00 = Non-cacheable, 01 = Write back cacheable, 10 = Write thru cacheable, 11 = Write Back Non-cacheable
        (0b01LL << 8)  | // IRGN0=1 write back .. options 00 = Non-cacheable, 01 = Write back cacheable, 10 = Write thru cacheable, 11 = Write Back Non-cacheable
        (0b0LL  << 7)  | // EPD0  ... Translation table walk disable for translations using TTBR0_EL1  0 = walk, 1 = generate fault
        (25LL   << 0);   // T0SZ=25 (512G)  ... The region size is 2 POWER (64-T0SZ) bytes
    asm volatile ("msr tcr_el1, %0; isb" : : "r" (r));
	
	// finally, toggle some bits in system control register to enable page translation
    asm volatile ("isb; mrs %0, sctlr_el1" : "=r" (r));
    r |= 0xC00800;     // set mandatory reserved bits
    r |= (1<<12) |     // I, Instruction cache enable. This is an enable bit for instruction caches at EL0 and EL1
           (1<<4)  |   // SA0, tack Alignment Check Enable for EL0
           (1<<3)  |   // SA, Stack Alignment Check Enable
           (1<<2)  |   // C, Data cache enable. This is an enable bit for data caches at EL0 and EL1
           (1<<1)  |   // A, Alignment check enable bit
           (1<<0);     // set M, enable MMU
    asm volatile ("msr sctlr_el1, %0; isb" : : "r" (r));


}

uint64_t virtualmap (uint32_t phys_addr, uint8_t memattrs) {
	uint64_t addr = 0;
	for (int i = 0; i < 512; i++)
	{
		if (TTBR1_Level3[i].RawData == 0) {							// Find the first vacant stage3 table slot
			uint64_t offset;
			TTBR1_Level3[i] = (Block_Descriptor) { .Address = (uintptr_t)phys_addr << (21 - 12), .AccessFlag = 1, .MemoryAttribute = memattrs, .EntryType = 3 };
			asm volatile ("dmb sy" ::: "memory");
			offset = ((512 - i) * 4096) - 1;
			addr = 0xFFFFFFFFFFFFFFFFul;
			addr = addr - offset;
			return(addr);
		}
	}
	return (addr);													// error
}