#include <Drivers/GPIO.h>
#include <Drivers/emmc.h>
#include <Drivers/peripherals.h>

#include <Libraries/printx.h>
#include <Libraries/types.h>
#include <Libraries/utils.h>
#include <Libraries/errors.h>

#include <stdbool.h>

static SDCard card;

/*  Sets the clock rate of the host controller to the closest frequency
    using power-of-2 division of the host controller base frequency and target
    frequency
    REQUIRES: unsigned integer target frequency freq
    RETURNS: 0 on success, -1 on failure*/
static int set_clock(uint32_t freq) {
    int host_ver = 3;
    int base_clock = 41666666;

    /* Calculated based on the table on page 47 of
       the SD Host Controller Simplified Specification Version 3.00
       partA2_300.pdf */
    int divisor = round_to_power2(ceiling(base_clock, freq)) >> 1;

    printf("EMMC: Target Frequency: %d, Closest frequency: %d\n", freq,
           base_clock / (divisor << 1));

    printf("SD Host Spec %d\n", SLOTISR->SD_VERSION + 1);

    /* Host Spec 1.0 and 2.0 have 8-bit, power-of-2 clock divider with
     * values between 0x0 and 0x80 */
    if (SLOTISR->SD_VERSION < HOST_SPEC_3) {
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
        CONTROL1->CLK_FREQ_LS8 = divisor & 0xFF;         // Bottom 8 birs
        CONTROL1->CLK_FREQ_MS2 = (divisor & 0x300) >> 8; // Top 2 bits
    }

    CONTROL1->CLK_INTLEN = true; // Enable internal EMMC clock

    TIMEOUT(!CONTROL1->CLK_STABLE);

    CONTROL1->CLK_EN = true;

    printf("Divisor set to 0x%x\n",
           (CONTROL1->CLK_FREQ_MS2 << 0xFF) | CONTROL1->CLK_FREQ_LS8);

    return 0;
}

static int command(u32 index, u32 arg) {
    // Clear interrupt
    INTERRUPT = INTERRUPT;

    printf("    CMD: %d\n", index);

    CMDTM_REG tmp;

    switch (index) {

    case 0:
        ARG1 = 0x0;
        CMDTM->CommandIndex = index;
        break;
    case 2:
        CMDTM->CommandResponseType = 0b01;
        CMDTM->CommandIndex = index;
        break;
    case 3:
        CMDTM->CommandResponseType = 0b10;
        CMDTM->CommandIndexCheckEnable = true;
        CMDTM->CommandCRCCheckEnable = true;
        CMDTM->CommandIndex = index;
        break;
    case 7:
        ARG1 = arg;
        CMDTM->CommandResponseType = 0b11;
        CMDTM->CommandIndex = index;
        break;
    case 8:
        ARG1 = arg;
        CMDTM->CommandResponseType = 0b10;
        CMDTM->CommandIndex = index;
        break;
    case 9:
        ARG1 = 0;
        CMDTM->CommandResponseType = 0b01;
        CMDTM->CommandIndex = index;
    case 17:
        ARG1 = arg;
        tmp.CommandResponseType = 0b10;
        tmp.CommandIsDataTransfer = true;
        tmp.TransferDataDirection = 1;
        tmp.CommandCRCCheckEnable = false;
        tmp.CommandIndex = index;
        tmp.TransferBlockCountEnable = true;
        *CMDTM = tmp;
        break;
    case 41:
        // Raspberry Pi only supports 3.3V OCR
        ARG1 = arg;
        CMDTM->CommandResponseType = 0b10;
        CMDTM->CommandIndex = index;
        break;
    case 55:
        ARG1 = arg;
        CMDTM->CommandResponseType = 0b10;
        CMDTM->CommandIndex = index;
        break;
    default:
        return -1;
    }

    TIMEOUT(!(INTERRUPT & 0x1));

    return 0;
}

uint64_t response() {
    printf("Response 0x%x %x %x %x\n", RESP0, RESP1, RESP2, RESP3);
    return RESP0;
}

static int init_card() {
    IRPT_EN = 0xffffffff;
    IRPT_MASK = 0xffffffff;

    RETRY(command(0, 0), 5);

    RETRY(command(8, 0x1AA), 5);

    int count = 20;
    do {
        RETRY(command(55, 0), 5);
        command(41, (1 << 30) | (1 << 20) | (1 << 21));
        delay(100000);
        card.initialized = GET_BITS(RESP0, 31, 31);
    } while (count-- && !card.initialized);

    if (!card.initialized)
        return -1;

    printf("Card Initialized, waiting for identification\n");

    RETRY(command(2, 0), 5);
    printf("Found CID: 0x%x, waiting for card address\n", response());
    RETRY(command(3, 0), 5);
    card.address = GET_BITS(RESP0, 31, 16);
    printf("Found RCA: 0x%x\n", card.address);
    response();

    command(9, card.address << 16);
    delay(1000000);
    printf("Status: 0x%x\n", STATUS);
    printf("Interrupt: 0x%x\n", INTERRUPT);
    response();

    command(7, card.address << 16);
    delay(1000000);
    response();
    return 0;
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

    TIMEOUT(CONTROL1->SRST_HC);

    CONTROL1->DATA_TOUNIT = 0xF - 1; // Maximum timeout delay for transfers
    delay(10000000);

    if (set_clock(400e3) == -1)
        return -1;

    delay(10000000);

    if (init_card()) {
        printf("EMMC Initialization Error\n");
        return -1;
    } else {
        printf("EMMC Initialization Success\n");
    }

    return 0;
}

int read_emmc(int address) {

TRY:
    BLKSIZECNT->block_count = 1;
    BLKSIZECNT->block_size = 512;
    command(17, address);
    response();

    int retry = 50;
    while(INTERRUPT != 0x21 && retry--){
        printf("Status: 0x%x\n", STATUS);
        printf("Interrupt: 0x%x\n", INTERRUPT);
    }

    INTERRUPT = INTERRUPT;

    if(retry <= 0)
        goto TRY;

    uint32_t buffer[512];

    printf("Status: 0x%x\n", STATUS);
    printf("Interrupt: 0x%x\n", INTERRUPT);

    delay(100000);
    int i = 0;
    while (i < 128){
        buffer[i++] = DATA;
        printf("%x ", buffer[i - 1]);
    }
    printf("\n");

    printf("Status: 0x%x\n", STATUS);
}