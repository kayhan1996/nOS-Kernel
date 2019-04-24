#include "uart.h"
#include "mmu.h"
#include "mm.h"
#include "printx.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	init_uart();
	init_printf(0, putc);
	init_mmu();
	init_memory_manager();

	printf("Hello World!");
}
