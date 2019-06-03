
#include "mmu.h"
#include "asm.h"
#include "uart.h"
#include "process.h"
#include "mm.h"
#include "printx.h"

#include "interrupts.h"
#include "timer.h"

#include "pretty_print.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void p1(char *str){
	uint64_t currentEL;
	printf("Current EL: %d\n", currentEL);
	while(1){
		printf(str);
		delay(1000000);
	}
}




void kernel_main(){
	init_uart();
	init_printf(0, putc);
	printf("Kernel Program Started.\n");
	clear();
	
	paint();

	init_memory_manager();
	init_process_manager();
	enable_interrupt_controller();
	init_arm_timer(300000000);
	enable_irq();

	create_process(p1, "abcde\n");
	create_process(p1, "     		12345\n");

	while(1){
	}
}