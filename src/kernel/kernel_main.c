#include "uart.h"
#include "mstdio.h"
#include "mailbox.h"
#include "mem.h"
#include "asm.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	init_uart();
	init_memory();

	println("Kernel Program Started");
	uint64_t address = 0x82000;
	uint64_t value;

	while (1) {
		uart_getc();
		print("Address: "); printhex(address++); print("  |  "); printhex(value); println("");
    }
}
