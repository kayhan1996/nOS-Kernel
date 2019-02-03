#include "mailbox.h"
#include "mstdio.h"

volatile unsigned int  __attribute__((aligned(16))) mailbox[36];


#define BASE        0x3F000000
#define VCMB        (BASE+0x0000B880)
#define STATUS      ((volatile unsigned int*)(VCMB+0x18))
#define READ        ((volatile unsigned int*)(VCMB+0x00))
#define RESPONSE    0x80000000
#define FULL        0x80000000
#define EMPTY       0X40000000

void doNothingWhile(unsigned int condition){
    println("Waiting");
    unsigned int count = 0;
    do{
        asm volatile("nop");
        printhex(condition);
        count++;
        if(count == 10){
            return;
        }
    }while(condition);
    println("Done Waiting");
}

int callMailBox(unsigned char ch){
    unsigned long r;

    //make all bits 1 except first 4
    r = (unsigned long)&mailbox & ~0xF;

    //set first 4 bits
    r = r | (ch&0xF);

    unsigned int read = (unsigned int)r;

    //do nothing while mailbox is full
    doNothingWhile(*STATUS & FULL);

    while(1) {
        doNothingWhile(*STATUS & EMPTY);
        if(read == *READ){
            return mailbox[1] == RESPONSE;
        }
        println("Unsuccesful Response");
    };
}