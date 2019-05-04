
#include "mmu.h"
#include "mm.h"
#include "printx.h"
#include "asm.h"
#include "LED.h"
#include "uart.h"
#include "mini_uart.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{

	init_mini_uart();
	
	mu_send('H');
	mu_send('E');
	mu_send('L');
	mu_send('L');
	mu_send('O');

	init_uart();
	send('U');
	send('R');
	send('T');
	act_led(On);
}
