
#include "mmu.h"
#include "mm.h"
#include "printx.h"
#include "asm.h"
#include "LED.h"
#include "uart.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	init_uart();
	init_printf(0, putc);
	printf("Hello Kernel!");
	LED_ACT_ON();
	while(1){
		send(receive());
	}
}
