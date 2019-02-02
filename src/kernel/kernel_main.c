#include "uart.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	// Declare as unused
	(void) r0;
	(void) r1;
	(void) atags;
 
	uart_init();
	uart_puts("Kernel Program Started\r\n");
 
	while (1) {
		uart_putc(uart_getc());
        uart_putc('\n');
		delay(200000000);
		uart_puts("Done\n");
    }
}
