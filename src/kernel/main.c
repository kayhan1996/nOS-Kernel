
#include "mmu.h"
#include "mm.h"
#include "asm.h"
#include "uart.h"
#include "printx.h"
#include "interrupts.h"
#include "timer.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	init_uart();
	init_printf(0, putc);
	
	enable_interrupt_controller();
	init_timer();

	asm("msr daifclr, #2");

	int i = 0;
	while(1){
		printf("Current Time: %d\n", get_current_time());
		printf("Next Timer: %d\n", get_next_time());
		timer_delay(10);
	}
}

void currentEL(){
	int currentEL;
	int	SPSel; 
	asm("mrs %[EL], currentEL" : [EL] "=r" (currentEL));
	asm("mrs %[SPSEL], SPSel" : [SPSEL] "=r" (SPSel));
	currentEL = currentEL >> 2;
	switch(currentEL){
		case 0:
			printf("EL0\n");
			break;
		case 1:
			printf("EL1");
			if(SPSel == 0)
				printf("t\n");
			else
				printf("h\n");
			break;
		case 2:
			printf("EL2\n");
			break;
		default:
			printf("Unknown state: %x", currentEL
	);
	}
}