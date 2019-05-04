#include "mailbox.h"
#include "stdint.h"

#define MAILBOX_BASE            0x3f00b880
#define STATUS                  (MAILBOX_BASE+0x18)
#define READ                    (MAILBOX_BASE+0x00)
#define WRITE                   (MAILBOX_BASE+0x20)
#define FULL                    0x80000000
#define EMPTY                   0x40000000
#define RESPONSE                0x80000000



int call_mailbox(Message *message){
    volatile uint32_t *status = STATUS;
    volatile uint32_t *write = WRITE;
    volatile uint32_t *read = READ;
    //format message
    uint32_t message_address = &(message->mailbox);
    message_address |= (message->channel & 0xF);    //add channel to message
    //end format

    //check if mailbox is full
    while(*status & FULL){asm("");}

    *write = message_address;

    while(1){
        while(*status & EMPTY){asm("");}
        if(*read == message_address){
            return message->mailbox[1] == RESPONSE;
        }
    }
}