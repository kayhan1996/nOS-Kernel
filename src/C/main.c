
#include "mmu.h"
#include "mm.h"
#include "asm.h"
#include "uart.h"
#include "printx.h"
#include "process.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(){
	init_uart();
	init_printf(0, putc);
	init_processes();

	while(1){
		counter_arm(1000000);
	}
}

void counter_arm(int n){
	register unsigned long f, t, r;
    // get the current counter frequency
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
	printf("Frequency: %d\n", f);
    // read the current counter
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
	printf("Current Counter: %d\n", t);
    // calculate expire value for counter
    t+=((f/1000)*n)/1000;
    do{asm volatile ("mrs %0, cntpct_el0" : "=r"(r));}while(r<t);

	register uint64_t tmp;
	asm("mrs %[tmp], CNTP_TVAL_EL0" : [tmp] "=r" (tmp));
	printf("TimerValue: %d\n", tmp);
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