#include <Drivers/GPIO.h>
#include <Drivers/emmc.h>
#include <Drivers/peripherals.h>
#include <Drivers/timer.h>

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

    CONTROL1->CLK_EN = false;
    delay_ms(10);

    /* Calculated based on the table on page 47 of
       the SD Host Controller Simplified Specification Version 3.00
       partA2_300.pdf */
    int divisor = round_to_power2(ceiling(base_clock, freq)) >> 1;

    log("EMMC: Target Frequency: %d, Closest frequency: %d\n", freq,
           base_clock / (divisor << 1));

    log("SD Host Spec %d\n", SLOTISR->SD_VERSION + 1);

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

    log("Divisor set to 0x%x\n",
           (CONTROL1->CLK_FREQ_MS2 << 0xFF) | CONTROL1->CLK_FREQ_LS8);

    return 0;
}

int command(u32 index, u32 arg) {
    if(index == 41) command(55, 0);
    // Clear interrupt
    INTERRUPT->Data = INTERRUPT->Data;

    log("    CMD: %d\n", index);

    ARG1 = arg;

    CMDTM_REG tmp = {0};

    tmp.CommandIndex = index;

    switch (index) {

    case 0:      
        break;
    case 2:
        tmp.CommandResponseType = 0b01;
        break;
    case 3:
        tmp.CommandResponseType = 0b10;
        break;
    case 7:
        tmp.CommandResponseType = 0b11;
        break;
    case 8:
        tmp.CommandResponseType = 0b10;
        break;
    case 9:
        tmp.CommandResponseType = 0b01;
        break;
    case 12:
        tmp.CommandResponseType = 0b11;
        break;
    case 16:
        tmp.CommandResponseType = 0b10;
        break;
    case 17:
        tmp.CommandResponseType = 0b10;
        tmp.CommandIsDataTransfer = true;
        tmp.TransferDataDirection = 1;
        break;
    case 18:
        tmp.CommandResponseType = 0b10;
        tmp.CommandIsDataTransfer = true;
        tmp.TransferDataDirection = 1;
        tmp.TransferAutoCommandEnable = 0b01;
        tmp.TransferBlockCountEnable = true;
        tmp.TransferMultipleBlocks = true;
    case 41:
        // Raspberry Pi only supports 3.3V OCR
        tmp.CommandResponseType = 0b10;
        break;
    case 55:
        tmp.CommandResponseType = 0b10;
        break;
    default:
        return -1;
    }

    *CMDTM = tmp;

    if(index == 8 || index == 55){
        delay_ms(100);
    }else if(index == 41){
        delay_ms(1000);
    }

    TIMEOUT(!(INTERRUPT->CMD_DONE));

    if(INTERRUPT->ERR) return -1;

    return 0;
}

uint64_t response() {
    log("Response 0x%x %x %x %x\n", RESP0, RESP1, RESP2, RESP3);
    return RESP0;
}

int init_CSD(){
    RETRY(command(9, card.address << 16), 7);
    response();

    uint32_t csd[4] = {RESP0, RESP1, RESP2, RESP3};

    int GET_CSD_BITS(int a, int b){
        int offset = a/32;
        int a_o = a % 32;
        int b_o = b % 32;

        if(b_o > a_o){
            int upper = GET_BITS(csd[offset]<<8, a_o, 0);
            int lower = GET_BITS(csd[offset-1]<<8, 31, b_o);
            int offset2 = 32-b_o;
            lower |= upper << offset2;
            return lower;
        }else{
            return GET_BITS(csd[offset]<<8, a_o, b_o);
        }
    }

    //High Capacity Card
    if(GET_CSD_BITS(127, 126) == 0x1){
        u32 C_SIZE = GET_CSD_BITS(69, 48);
        card.sector_count = (C_SIZE+1);
        card.capacity = (C_SIZE+1)*(512*1024);
        card.sector_size = 512;
        return 0;
    }
    //Standard Capacity Card
    else{
        printf("Found SDSC Card. Card type not yet supported\n");
        return -1;
    }
}

static int init_card() {
    IRPT_EN = 0xffffffff;
    IRPT_MASK = 0xffffffff;

    RETRY(command(0, 0), 5);

    RETRY(command(8, 0x1AA), 5);

    int count = 20;
    do {
        delay(500);
        command(41, 0x51ff8000);
        card.initialized = GET_BITS(RESP0, 31, 31);
    } while (count-- && !card.initialized);

    if (!card.initialized)
        return -1;

    log("Card Initialized, waiting for identification\n");

    RETRY(command(2, 0), 5);
    log("Found CID: 0x%x, waiting for card address\n", response());
    RETRY(command(3, 0), 5);
    card.address = GET_BITS(RESP0, 31, 16);
    log("Found RCA: 0x%x\n", card.address);
    RETRY(init_CSD(), 3);
    RETRY(command(7, card.address << 16), 7);

    set_clock(20e6);

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
    delay(1);    

    if (set_clock(400e3) == -1)
        return -1;

    delay(1);

    if (init_card()) {
        printf("EMMC Initialization Error\n");
        return -1;
    } else {
        printf("EMMC Initialization Success\n");
    }

    return 0;
}

int read_emmc(uint32_t address, uint32_t count, unsigned char *bytes){

    TIMEOUT((STATUS->CMD_INHIBIT | STATUS->DAT_INHIBIT));

    BLKSIZECNT->block_size = 512;
    BLKSIZECNT->block_count = count;
       
    delay(1);

    command(18, address);

    uint32_t *buf = (uint32_t*)bytes;

    int transferedBlocks = count;
    while(transferedBlocks--){
        TIMEOUT(!(INTERRUPT->READ_RDY));
        INTERRUPT->READ_RDY = 1;
        for(int i = 0; i < 128; i++){
            buf[i] = DATA;
        }
        buf += 128;
    }

    return 0;
}

int status_emmc(){
    if(card.initialized){
        return 0;
    }else{
        return -1;
    }
}

#include "Filesystem/FAT32/diskio.h"
int ioctl_emmc(uint32_t cmd, void* buff){
    if(cmd == CTRL_SYNC){
        TIMEOUT(!(STATUS->WRITE_TRANSFER));
        return 0;
    }else if(cmd == GET_SECTOR_COUNT){
        *(uint32_t*)buff = card.sector_count;
        return 0;
    }else if(cmd == GET_SECTOR_SIZE){
        *(uint32_t*)buff = card.sector_size;
    }else if(cmd == GET_BLOCK_SIZE){
        return 1;
    }else if(cmd == CTRL_TRIM){
        return 0;
    }else{
        return -1;
    }
}