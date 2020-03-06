#ifndef __MMU__
#define __MMU__

#include "stdint.h"

#define BLOCK_SIZE 2
#define PERIPHERALS 16
#define VRAM 32
#define RAM 1024

#define PERIPHERALS_MAX (RAM / BLOCK_SIZE)
#define VRAM_MAX (PERIPHERALS_MAX - (PERIPHERALS / BLOCK_SIZE))
#define RAM_MAX (VRAM_MAX - (VRAM / BLOCK_SIZE))

#define Table_Shift_4kB 12 // since tables are aligned 2^12 = 4096 bytes
#define Table_Shift_2MB 9  // (2^12)(2^9) = 2MB
#define Table_shift_1GB 9  // (2^12)(2^9)(2^9) = 1GB
#define Block_Shift_4kB 12 // since blocks are aligned 2^12 = 4096 bytes
#define Block_Shift_2MB                                                        \
    9 // since blocks are 2MB aligned so (2^12) x (2^9) = 2MB

uint64_t *map_physical_to_virtual(uintptr_t address);

typedef enum type { Table = 3, Page = 3, Block = 1, Invalid_Block = 0 } Type;

typedef enum ap {
    AP_NA_RW = 0,
    AP_RW_RW = 1,
    AP_NA_RO = 2,
    AP_RO_RO = 3
} Access_Permissions;

typedef enum aptable {
    //[USER]_[SYSTEM] permissions
    APTable_RW_RW = 0,
    APTable_NA_RW = 1,
    APTable_RO_RO = 2,
    APTable_NA_RO = 3
} APTable;

/* More Information Available at:
   http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0500d/B4BEIDGHFH.html*/
typedef enum ma {
    Device_nGnRnE = 0,
    Device_nGnRE = 1,
    Device_GRE = 2,
    Normal_nC = 3,
    Normal = 4
} Memory_Attributes;

typedef enum sh {
    Non_Shareable = 0,
    Invalid = 1,
    Outer = 2,
    Inner = 3
} Shareable;

typedef enum ca {
    Non_Cacheable = 0,
    Write_Back_Cacheable = 1,
    Write_Through_Cacheable = 2,
    Write_Back_Non_Cacheable = 3
} Cacheable;

typedef union {
    struct __attribute__((__packed__)) {
        Type type : 2;
        uint64_t ignored : 10;
        uint64_t address : 36;
        uint64_t reserved : 4;
        uint64_t software_reserved : 7;
        uint64_t PXNTable : 1;
        uint64_t XNTable : 1;
        Access_Permissions APTable : 2;
        uint64_t NSTable : 1;
    };
    uint64_t data;
} Table_Descriptor;

typedef union {
    struct __attribute__((__packed__)) {
        Type type : 2;                           //[1:0]
        Memory_Attributes memory_attributes : 4; //[5:2]
        Access_Permissions AP : 2;               //[6:7]
        Shareable SH : 2;                        //[9:8]
        uint64_t AF : 1;                         //[10]
        uint64_t reserved0 : 1;                  //[11]
        uint64_t address : 36;                   //[47:12]
        uint64_t reserved1 : 4;                  //[51:48]
        uint64_t contiguous : 1;                 //[52]
        uint64_t reserved3 : 1;                  //[53]
        uint64_t XN : 1;                         //[54]
        uint64_t reserved4 : 9;                  //[63:55]
    };
    uint64_t data;
} Block_Descriptor;

typedef union {
    struct __attribute__((__packed__)) {
        Type type : 2;                           //[1:0]
        uint64_t AttrIndx : 3;                   //[4:2]
        uint64_t NS : 1;                         //[5]
        Access_Permissions AP : 2;               //[6:7]
        Shareable SH : 2;                        //[9:8]
        uint64_t AF : 1;                         //[10]
        uint64_t reserved0 : 1;                  //[11]
        uint64_t address : 36;                   //[47:12]
        uint64_t reserved1 : 4;                  //[51:48]
        uint64_t contiguous : 1;                 //[52]
        uint64_t reserved3 : 1;                  //[53]
        uint64_t XN : 1;                         //[54]
        uint64_t reserved4 : 9;                  //[63:55]
    };
    uint64_t data;
} Page_Descriptor;

typedef struct __attribute__((__packed__)) {
    enum { T0SZ_512 = 25 } T0SZ : 6;

    uint64_t reserved0 : 1;
    uint64_t EPD0 : 1;
    Cacheable IRGN0 : 2;
    Cacheable ORGN0 : 2;
    Shareable SH0 : 2;

    enum { TG0_4 = 0, TG0_16 = 2, TG0_64 = 1 } TG0 : 2;

    enum { T1SZ_512 = 25 } T1SZ : 6;

    uint64_t A1 : 1; // 0bx TTBRx_EL1.ASID defines ASID
    uint64_t EPD1 : 1;
    Cacheable IRGN1 : 2;
    Cacheable ORGN1 : 2;
    Shareable SH1 : 2;

    enum {
        TG1_4 = 2,
        TG1_16 = 1,
        TG1_64 = 3
    } TG1 : 2; // different values from TG0

    enum { IPS_32 = 0, IPS_36 = 1, IPS_40 = 2 } IPS : 3;

    uint64_t reserved1 : 1;

    enum { AS_8 = 0, AS_16 = 1 } AS : 1;

    uint64_t TBI0 : 1;
    uint64_t TBI1 : 1;

    uint64_t reserved3 : 25;
} TCR_EL1;

typedef union {

    struct __attribute__((__packed__)) {
        uint64_t enable_MMU : 1;
        uint64_t enable_alignment_fault_check : 1;
        uint64_t enable_data_cache : 1;
        uint64_t enable_stack_alignment_check : 1;
        uint64_t enable_stack_el0_alignment_check : 1;
        uint64_t reserved0 : 6;
        uint64_t enable_instruction_cache : 1;
        uint64_t reserved1 : 52;
    };

    uint64_t MandatoryReserved1Bits;
} SCTLR_EL1;

__attribute__((aligned(4096))) Block_Descriptor T0_L2[512];
__attribute__((aligned(4096))) Table_Descriptor T0_L1[512];

__attribute__((aligned(4096))) Block_Descriptor T1_L3[512];
__attribute__((aligned(4096))) Table_Descriptor T1_L2[512];
__attribute__((aligned(4096))) Table_Descriptor T1_L1[512];

#endif /* __PAGE_TABLE__ */