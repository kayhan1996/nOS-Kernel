#ifndef __EMMC_H__
#define __EMMC_H__

#include <Libraries/types.h>

int init_emmc();

int read_emmc(uint32_t address, uint32_t count, unsigned char *bytes);

int status_emmc();

int ioctl_emmc(uint32_t cmd, void* buff);

#define HOST_SPEC_3 2

/* Version information and Slot Interrupt Status */
struct SlotISR {
    u32 SLOT_STATUS : 8;
    u32 res0 : 8;
    u32 SD_VERSION : 8;
    u32 VENDOR : 8;
};

typedef union {
    struct {
        u32 res0 : 1;
        u32 DWIDTH_4 : 1; // Use 4 data lines
        u32 HS_EN : 1;    // Use high speed mode
        u32 res1 : 2;
        u32 DWIDTH_8 : 1; // Use 8 data lines
        u32 res2 : 10;
        u32 GAP_STOP : 1;    // Stop current transaction at next block gap
        u32 GAP_RESTART : 1; // Restart transaction stopped using GAP_STOP
        u32 READWAIT_EN : 1; // Use DAT2 read-wait protocol for supported cards
        u32 GAP_IEN : 1;  // Enable SDIO interrupt at block gap (only valid if
                          // DWIDTH_4 set)
        u32 SPI_MODE : 1; // SPI mode enable
        u32 BOOT_EN : 1;  // Boot mode access
        u32 ALT_BOOT_EN : 1; // Alternative boot mode access
        u32 res3 : 9;
    };
    u32 Data;
} Control0;

typedef union {
    struct {
        u32 CLK_INTLEN : 1; // Clock enable for internal EMMC clocks power
                            // saving
        u32 CLK_STABLE : 1; // SD clock stable
        u32 CLK_EN : 1;     // SD clock enable
        u32 res0 : 2;
        u32 CLK_GENSEL : 1;   // Clock generation mode 0=divided, 1=programmable
        u32 CLK_FREQ_MS2 : 2; // base divider MSB
        u32 CLK_FREQ_LS8 : 8; // base divider LSB
        u32 DATA_TOUNIT : 4;  // data timeout unit exponent 1111=disabled, x =
                              // TMCLK * 2^(x+13)
        u32 res1 : 4;
        u32 SRST_HC : 1;   // Completely reset host circuit
        u32 SRST_CMD : 1;  // Reset command handling circuit
        u32 SRST_DATA : 1; // Reset data handling circuit
        u32 res2 : 5;
    };
    u32 Data;
} Control1;

typedef union cmd_reg {
    struct {
        u32 res0 : 1;
        u32 TransferBlockCountEnable : 1; // Enable the block count for multiple block transfers
        u32 TransferAutoCommandEnable : 2; // Select the command to be sent after completion of data transfer
        u32 TransferDataDirection : 1;     // 0=host to card | 1=card to host
        u32 TransferMultipleBlocks : 1;    // 0=single block | 1=multiple blocks
        u32 res1 : 10;
        u32 CommandResponseType : 2; // 00=none | 01=136-bit | 10=48-bit | 11=48-bits using busy
        u32 res2 : 1;
        u32 CommandCRCCheckEnable : 1;   // Check response's CRC
        u32 CommandIndexCheckEnable : 1; // Check response has same index as command
        u32 CommandIsDataTransfer : 1;   // 0=no data transfer | 1=data transfer
        u32 CommandType : 2;  // 00=normal | 01=suspend current | 10=resume last | 11=abort current
        u32 CommandIndex : 6; // Issued command index
        u32 res3 : 2;
    };
    u32 Data;
} CMDTM_REG;

typedef union {
    struct {
        u32 CMD_DONE : 1;   //Command has finished
        u32 DATA_DONE : 1;  //Data transfer has finished
        u32 BLOCK_gap : 1;  //Data transfer has stopped at block gap
        u32 res0 : 1;
        u32 WRITE_RDY : 1;  //Data can be written to DATA register
        u32 READ_RDY : 1;   //DATA register contains data to be read
        u32 res1 : 2;
        u32 CARD : 1;       //Card made interrupt request
        u32 res2 : 3;
        u32 RETUNE : 1;     //Clock retune request was made
        u32 BOOTACK : 1;    //Boot acknowledge has been received
        u32 ENDBOOT : 1;    //Boot operation has terminated
        u32 ERR : 1;        //An error has occured
        u32 CTO_ERR : 1;    //Timeout on command line
        u32 CCRC_ERR : 1;   //Command CRC error
        u32 CEND_ERR : 1;   //End bit on command line not 1
        u32 CBAD_ERR : 1;   //Incorrect command index in response
        u32 DTO_ERR : 1;    //Timeout on data line
        u32 DCRC_ERR : 1;   //Data CRC error
        u32 DEND_ERR : 1;   //End bit on data line not 1
        u32 res3 : 1;
        u32 ACMD_ERR : 1;   //Auto command error
        u32 res4 : 7;
    };
    u32 Data;
} interrupt_reg;

typedef union {
    struct {
        u32 CMD_INHIBIT : 1; //Command line still used by previous command
        u32 DAT_INHIBIT : 1; //Data lines still used by previous data transfer
        u32 DAT_ACTIVE : 1; //At least one data line is active
        u32 res0 : 5;
        u32 WRITE_TRANSFER : 1; //New data can be written to EMMC
        u32 READ_TRANSFER : 1; //New data can be read from EMMC
        u32 res1 : 10;
        u32 DAT_LEVEL0 : 4; //Value of data lines DAT3 to DAT0
        u32 CMD_LEVEL : 1;  //Value of command line CMD
        u32 DAT_LEVEL1 : 4; //Value of data lines DAT7 to DAT4
        u32 res2 : 3;
    };
    u32 Data;
} status_reg;

typedef union {
    struct {
        u32 block_size : 10;
        u32 res : 6;
        u32 block_count : 16;
    };
    u32 Data;
} block_size_count_reg;

typedef struct {
    u32 initialized : 1;
    u32 address : 16;
    u32 capacity;
    u32 sector_count;
    u32 sector_size;
} SDCard;

#define BLKSIZECNT ((volatile block_size_count_reg *)(EMMC_BASE + 0x4))
#define CMDTM ((volatile CMDTM_REG *)(EMMC_BASE + 0xc))
#define STATUS ((volatile status_reg *)(EMMC_BASE + 0x24))
#define CONTROL0 ((volatile Control0 *)(EMMC_BASE + 0x28))
#define CONTROL1 ((volatile Control1 *)(EMMC_BASE + 0x2c))
#define SLOTISR ((volatile struct SlotISR *)(EMMC_BASE + 0xfc))
#define INTERRUPT ((volatile interrupt_reg *)(EMMC_BASE + 0x30))


#define ARG1 (*(volatile u32 *)(EMMC_BASE + 0x8))
#define ARG2 (*(volatile u32 *)(EMMC_BASE + 0x0))

#define RESP0 (*(volatile u32 *)(EMMC_BASE + 0x10))
#define RESP1 (*(volatile u32 *)(EMMC_BASE + 0x14))
#define RESP2 (*(volatile u32 *)(EMMC_BASE + 0x18))
#define RESP3 (*(volatile u32 *)(EMMC_BASE + 0x1c))

#define DATA (*(volatile u32 *)(EMMC_BASE + 0x20))


#define IRPT_MASK (*(volatile u32 *)(EMMC_BASE + 0x34))
#define IRPT_EN (*(volatile u32 *)(EMMC_BASE + 0x38))


#endif /* __EMMC_H__ */