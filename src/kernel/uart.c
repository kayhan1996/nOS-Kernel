#include "uart.h"
#include "stdint.h"
#include "mailbox.h"
#include "asm.h"

#include "mini_uart.h"

#define BASE	0x3f201000
#define IO_BASE             0x3f000000
#define GPFSEL1             ((volatile uint32_t*)(IO_BASE+0x00200004))
#define GPPUD               ((volatile uint32_t*)(IO_BASE+0x00200094))
#define GPPUDCLK0           ((volatile uint32_t*)(IO_BASE+0x00200098))
#define TX_FULL				(1 << 5)
#define RX_EMPTY			(1 << 4)

struct UART_PL011 {
	volatile uint32_t data;
	volatile uint32_t RSRECR;
	uint32_t _res0[4];
	volatile uint32_t flag;
	uint32_t _res1;
	volatile uint32_t IBRD;
	volatile uint32_t FBRD;
	volatile uint32_t LCRH;
	volatile uint32_t CR;
	volatile uint32_t IFLS;
	volatile uint32_t IMSC;
	volatile uint32_t RIS;
	volatile uint32_t MIS;
	volatile uint32_t ICR;
} __attribute__((__packed__));

#define	PL011	((struct UART_PL011 *) BASE)

void init_uart()
{
	PL011->CR = 0;

	Message message;
	message.mail[0] = 9*4;		//message size
	message.mail[1] = 0; 		//request
	message.mail[2] = 12; 		//buffer size
	message.mail[3] = 8; 		//data size
	message.mail[4] = 2;		//device id for uart clock
	message.mail[5] = 4000000; 	//clock value = 4Mhz
	message.mail[6] = 0;		//end tag
	message.channel = 8;
	call_mailbox(&message);

	//reassign pins to UART0
	uint32_t pins = *GPFSEL1;
    pins &= ~(7 << 12); //disable pin 14
    pins &= ~(7 << 15); //disable pin 15
    pins |= (4 << 12);  //enable alternative function pin 14
    pins |= (4 << 15);
    *GPFSEL1 = pins;
    *GPPUD = 0;
    delay(150);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    delay(150);
    *GPPUDCLK0 = 0;

	PL011->ICR = 0x7FE;		//clear all interrupts
	PL011->IBRD = 2;		//115200 baud for 4Mhz clock
	PL011->FBRD = 0xB;
	PL011->LCRH = (1 << 5) | (1 << 6);	//8 bit characters
	PL011->CR = (1 << 0) | (1 << 8) | (1 << 9); // UARTEN | TXE | RXE
}

void send(char c)
{
	while (PL011->flag & TX_FULL) {asm("nop");}
	PL011->data = c;
}

char receive()
{
    while (PL011->flag & RX_EMPTY) {asm("nop");}
    return PL011->data;
}



void putc(void* p, char c){
	send(c);
}