#include <Drivers/GPIO.h>
#include <Drivers/emmc.h>
#include <Drivers/peripherals.h>

#include <Libraries/utils.h>
#include <Libraries/printx.h>

#include <stdbool.h>

/* Version information and Slot Interrupt Status */
struct SlotISR {
    u32 SLOT_STATUS : 8;
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

#define SLOTISR ((volatile struct SlotISR *)(EMMC_BASE + 0xfc))
#define CONTROL0 ((volatile Control0 *)(EMMC_BASE + 0x28))
#define CONTROL1 ((volatile Control1 *)(EMMC_BASE + 0x2c))

/*  Sets the clock rate of the host controller to the closest frequency
    using power-of-2 division of the host controller base frequency and target
    frequency
    REQUIRES: unsigned integer target frequency freq */
int set_clock(uint32_t freq) {
    int host_ver = 3;
    int base_clock = 41666666;

    /* Calculated based on the table on page 47 of
       the SD Host Controller Simplified Specification Version 3.00
       partA2_300.pdf */
    int divisor = round_to_power2(ceiling(base_clock, freq)) >> 1;

    printf("Target: %d, Closest frequency: %d\n", freq,
           base_clock / (divisor << 1));

    printf("SD Host Spec %d\n", SLOTISR->SD_VERSION);

    /* Host Spec 1.0 and 2.0 have 8-bit, power-of-2 clock divider with
     * values between 0x0 and 0x80 */
    if (SLOTISR->SD_VERSION <= 2) {
        if (divisor > 0x80) {
            divisor = 0x80;
        }

        CONTROL1->CLK_FREQ_LS8 = divisor;
    } else {
        /* Assume Host Spec 3.0 with 10-bit, power-of-2 clock
           divider with values between 0x0 and 0x3FF */
        if (divisor > 0x3FF) {
            divisor = 0x3FF;
        }

        /* Set 10-bit upper and lower bits of the divisor */
        CONTROL1->CLK_FREQ_LS8 = divisor & 0xFF;
        CONTROL1->CLK_FREQ_MS2 = divisor & 0x300;
    }

    printf("Divisor set to 0x%x\n", (CONTROL1->CLK_FREQ_MS2 << 0xFF) | CONTROL1->CLK_FREQ_LS8);

    return divisor;
}

int init_emmc() {
    /* Initialize GPIO pins [48:53] that connect to SD0_x controller*/
    for (int pin = 48; pin <= 53; pin++) {
        GPIO_set_function(pin, Alt3);
        GPIO_set_pull_mode(pin, Up);
    }

    /* Reset Host Circuit */
    CONTROL0->Data = 0;
    CONTROL1->Data = 0;
    CONTROL1->SRST_HC = true;

    int cnt = 10000000;
    while (CONTROL1->SRST_HC && cnt) {
        cnt--;
    }
    if (CONTROL1->SRST_HC)
        return -1;

    CONTROL1->CLK_INTLEN = true;     // Enable internal EMMC clock
    CONTROL1->DATA_TOUNIT = 0xF - 1; // Maximum timeout delay for transfers
    delay(100000000);

    set_clock(400e3);
    delay(100000000);
}