#include <Drivers/emmc.h>
#include <Drivers/GPIO.h>
#include <Drivers/peripherals.h>

#include <assert.h>

/* Version information and Slot Interrupt Status */
struct SlotISR {
    u32 SLOT_STATUS : 8;
    u32 SD_VERSION : 8;
    u32 VENDOR : 8;
};

struct Control0 {
    u32 res0        : 1;
    u32 DWIDTH_4    : 1;     // Use 4 data lines
    u32 HS_EN       : 1;     // Use high speed mode
    u32 res1        : 2;
    u32 DWIDTH_8    : 1;     // Use 8 data lines
    u32 res2        : 10;
    u32 GAP_STOP    : 1;     // Stop current transaction at next block gap
    u32 GAP_RESTART : 1;     // Restart transaction stopped using GAP_STOP
    u32 READWAIT_EN : 1;     // Use DAT2 read-wait protocol for supported cards
    u32 GAP_IEN     : 1;     // Enable SDIO interrupt at block gap (only valid if DWIDTH_4 set)
    u32 SPI_MODE    : 1;     // SPI mode enable
    u32 BOOT_EN     : 1;     // Boot mode access
    u32 ALT_BOOT_EN : 1;     // Alternative boot mode access
    u32 res3        : 9;
};

struct Control1 {
    u32 CLK_INTLEN  : 1;     // Clock enable for internal EMMC clocks power saving
    u32 CLK_STABLE  : 1;     // SD clock stable
    u32 CLK_EN      : 1;     // SD clock enable
    u32 res0        : 2;     
    u32 CLK_GENSEL  : 1;     // Clock generation mode 0=divided, 1=programmable
    u32 CLK_FREQ_MS2: 2;     // base divider MSB
    u32 CLK_FREQ_8  : 8;     // base divider LSB
    u32 DATA_TOUNIT : 4;     // data timeout unit exponent 1111=disabled, x = TMCLK * 2^(x+13)
    u32 res1        : 4;     
    u32 SRST_HC     : 1;     // Completely reset host circuit
    u32 SRST_CMD    : 1;     // Reset command handling circuit
    u32 SRST_DATA   : 1;     // Reset data handling circuit
    u32 res2        : 5;
};

#define SLOTISR         (volatile (struct SlotISR*)(EMMC_BASE+0xfc))
#define CONTROL0        (volatile (struct Control0*)(EMMC_BASE+0x28))
#define CONTROL1        (volatile (struct Control1*)(EMMC_BASE+0x2c))

static_assert(sizeof(struct Control0) == 4, "Control0 not 4 bytes");
static_assert(sizeof(struct Control1) == 4, "Control1 not 4 bytes");

int init_emmc(){
    struct Control1 c1;

    /* Initialize GPIO pins that connect to SD0_x controller*/
    for(int pin = 48; pin <= 53; pin++){
        GPIO_set_function(pin, Alt0);
        GPIO_set_pull_mode(pin, Up);
    }
}