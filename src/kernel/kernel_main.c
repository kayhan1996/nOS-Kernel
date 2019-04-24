#include "uart.h"
#include "mstdio.h"
#include "mailbox.h"
#include "mem.h"
#include "asm.h"
#include "debug.h"
#include "mmu.h"
#include "printx.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	
	uint64_t system_control;
	init_uart();
	init_printf(0, putc);
	tfp_printf("htps");
}
