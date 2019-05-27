
#include "mmu.h"
#include "asm.h"
#include "uart.h"
#include "printx.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(){
	init_uart();
	init_printf(0, putc);
	printf("Kernel Program Started.\n");
	while(1){
	}
}