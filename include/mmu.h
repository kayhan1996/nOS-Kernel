#include <stdint.h>

#define MT_DEVICE_NGNRNE	0
#define MT_DEVICE_NGNRE		1
#define MT_DEVICE_GRE		2
#define MT_NORMAL_NC		3
#define MT_NORMAL		    4

typedef union {
    struct __attribute__((__packed__)){
        uint64_t EntryType: 2;
        enum{
            DEVICE_NGNRNE = 0,
            DEVICE_NGNRE = 1,
            DEVICE_GRE = 2,
            NORMAL_NC = 3,
            NORMAL = 4
        } MemoryAttribute: 4;

        enum {
            NOREAD_EL0 = 1,
            NOWRITE_EL0 = 2
        } S2AP: 2;

        enum {
            OUTER_SHAREABLE = 2,
            INNER_SHAREABLE = 3
        } SH: 2;

        uint64_t AccessFlag: 1;
        uint64_t reserved11: 1;
        uint64_t Address: 36;
        uint64_t reserved48_51: 4;
        uint64_t Contiguous: 1;
        uint64_t reserved53: 1;
        uint64_t NoExecute: 1;
        uint64_t reserved55_63: 9;
    };
    uint64_t RawData;
} Block_Descriptor;

typedef union {
    struct __attribute__((__packed__)){
        uint64_t EntryType: 2;
        uint64_t reserved2_11: 10;
        uint64_t Address: 36;
        uint64_t reserved48_58: 11;
        uint64_t PXNTable: 1;
        uint64_t XNTable: 1;
        enum {
            NOEFFECT = 0,
            NO_EL0 = 1,
            NO_WRITE = 2,
            NO_WRITE_EL0_READ = 3
        } APTable: 2;
        uint64_t NSTable: 1;
    };
    uint64_t RawData;
} Table_Descriptor;