#include "Drivers/uart.h"
#include "stdint.h"
#include "Drivers/mailbox.h"
#include "Drivers/asm.h"
#include "Drivers/GPIO.h"
#include "Drivers/peripherals.h"

#define UART0_BASE 		(GPIO_BASE + 0x1000)
#define RX_EMPTY		(1 << 4)
#define TX_FULL			(1 << 5)

struct UART0{				//offset
	volatile uint32_t DATA;	//0x00
	uint32_t	  _res0[5];	//0x04, 0x08, 0x0c, 0x10, 0x14
	volatile uint32_t FLAG; //0x18
	uint32_t	  _res1[2];	//0x1c, 0x20
	volatile uint32_t IBRD;	//0x24
	volatile uint32_t FBRD; //0x28
	volatile uint32_t LCRH;	//0x2c
	volatile uint32_t CR;   //0x30
	volatile uint32_t IFLS; //0x34
	volatile uint32_t IMSC; //0x38
	volatile uint32_t RIS;	//0x3c
	volatile uint32_t MIS;  //0x40
	volatile uint32_t ICR;  //0X44
};

#define PL011	((struct UART0*) UART0_BASE)



void init_uart(){
	PL011->CR = 0;

	uint32_t message[16];
	message[0] = 8*4;
    message[1] = 0;
    message[2] = 0x38002; // set clock rate
    message[3] = 12;
    message[4] = 8;
    message[5] = 2;           // UART clock
    message[6] = 4000000;     // 4Mhz
    message[7] = 0;           // end tag
    call_mailbox(message, 8);

	// register uint32_t address;
	// address = GPIO->FSEL1;
    // address &= ~((7<<12)|(7<<15));	//set pins
    // address |=(4<<12)|(4<<15);		//alt0 mode
    // GPIO->FSEL1 = address;

    // GPIO->PUD = 0;            
    // delay(150);
    // GPIO->PUDCLK0 = (1<<14)|(1<<15);
    // delay(150);
    // GPIO->PUDCLK0 = 0;

	GPIO_set_function(7, Alt0);
	GPIO_set_function(15, Alt0);
	GPIO_set_pull_mode(7, Off);
	GPIO_set_pull_mode(15, Off);   

	PL011->ICR = 0x7FF;
	PL011->IBRD = 2;
	PL011->FBRD = 0xB;
	PL011->LCRH = (1 << 5) | (1 << 6);	//8 bits
	PL011->CR = (1 << 0) | (1 << 8) | (1 << 9); //UARTEN | TXE | RXE
}

void send(char c){
	while (PL011->FLAG & TX_FULL) {asm("nop");}
	PL011->DATA = c;
}

char receive(){
    while (PL011->FLAG & RX_EMPTY) {asm("nop");}
    return PL011->DATA;
}

void putc(void* p, char c){
	send(c);
}