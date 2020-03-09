#include "Drivers/uart_mini.h"
#include "Drivers/asm.h"
#include "Drivers/peripherals.h"

#define BASE                (GPIO_BASE + 0x15000)
#define IRQ_STATUS          ((volatile uint32_t*)(BASE+0x00))
#define ENABLE              ((volatile uint32_t*)(BASE+0x04))
#define DATA                ((volatile uint32_t*)(BASE+0x40))
#define INTERRUPT_ENABLE    ((volatile uint32_t*)(BASE+0x44))
#define INTERRUPT_ID        ((volatile uint32_t*)(BASE+0x48))
#define LINE_CONTROL        ((volatile uint32_t*)(BASE+0x4c))
#define MODEM_CONTROL       ((volatile uint32_t*)(BASE+0x50))
#define LINE_STATUS         ((volatile uint32_t*)(BASE+0x54))
#define MODEM_STATUS        ((volatile uint32_t*)(BASE+0x58))
#define EXTRA_CONTROL       ((volatile uint32_t*)(BASE+0x60))
#define BAUDRATE            ((volatile uint32_t*)(BASE+0x68))

#define TX_EMPTY            0x20
#define RX_READY            0x00

void init_mini_uart(){
    *ENABLE |= 0x1;
    *INTERRUPT_ID = 0xc6;     //disable interrupts
    *INTERRUPT_ENABLE = 0;   //enable FIFO and clear transmit and receive FIFOs
    *LINE_CONTROL = 0x3;     //8-bit mode
    *EXTRA_CONTROL = 0;     //disable tx and rx for pin setup
    *MODEM_CONTROL = 0;
    *BAUDRATE = 270;

    

    uint32_t pins = *GPFSEL1;
    pins &= ~(7 << 12); //disable pin 14
    pins &= ~(7 << 15); //disable pin 15
    pins |= (2 << 12);  //enable alternative function pin 14
    pins |= (2 << 15);
    *GPFSEL1 = pins;

    
    *GPPUD = 0;
    delay(150);

    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    delay(150);

    *GPPUDCLK0 = 0;
    *EXTRA_CONTROL = 3; //enable rx, tx, and auto flow control
}

void mu_send(char c) {
    while(!(*LINE_STATUS & TX_EMPTY)){asm("");}
    *DATA = c;
}

char mu_receive() {
    while(!(*LINE_STATUS & RX_READY)){asm("");}
    return *DATA;
}

