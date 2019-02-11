#include "uart.h"
#include "mstdio.h"
#include "mailbox.h"
#include "mem.h"
#include "asm.h"
#include "debug.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	init_uart();
	init_memory();

	println("Kernel Program Started");
	char flag;
	while (1) {
		println("\n\n\n\n\n_________________________________________");
		println("Select program to run: ");
		println("A) Memory Explorer");
		println("B) Page Explorer");
		flag = uart_getc();
		
		if(flag == 'a' || flag == 'A'){
			traverse_list();
		}else if(flag == 'b' || flag == 'B'){
			print_memory(NULL);
		}

    }
}
