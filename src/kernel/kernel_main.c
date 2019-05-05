
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
	while(1){
		send(receive());
	}
}
