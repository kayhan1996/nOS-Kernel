#include "mailbox.h"

#define MAILBOX_BASE        0x3F00B880
#define STATUS              ((volatile uint32_t*)(MAILBOX_BASE+0x18))
#define READ                ((volatile uint32_t*)(MAILBOX_BASE+0x00))
#define WRITE               ((volatile uint32_t*)(MAILBOX_BASE+0x20))

#define FULL                0x80000000
#define EMPTY               0x40000000
#define RESPONSE            0x80000000

int call_mailbox(uint32_t* message, uint8_t channel){

    //prepare message for channel
    uint32_t address = (uint32_t)message;
    address &= ~(0xF);
    address |= (channel & 0xF);

    //check mailbox status
    while(*STATUS & FULL){asm("");}

    *WRITE = address;

    while(1) {
        while(*STATUS & EMPTY){asm("");}
        if(address = *READ){
            return message[1] == RESPONSE;
        }
    }
}