#include "uart.h"

void uart_putc(unsigned char c)
{
	// Wait for UART to become ready to transmit.
	while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	mmio_write(UART0_DR, c);
}

void uart_puts(const char* str)
{
	for (unsigned int i = 0; str[i] != '\0'; i ++)
		uart_putc((unsigned char)str[i]);
}

unsigned char uart_getc()
{
    // Wait for UART to have received something.
    while (mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}

void init_uart()
{
	
}

void putc(void* p, char c){
	uart_putc(c);
}