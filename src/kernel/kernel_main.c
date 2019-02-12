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

	allocate_page();
	allocate_page();
	allocate_page();

	println("Kernel Program Started");
	char flag;
	while (1) {
		println("\n_________________________________________");
		println("Select program to run: ");
		println("A) Memory Explorer");
		println("B) Page Explorer");
		flag = uart_getc();
		
		if(flag == 'a' || flag == 'A'){
			print_memory(NULL);
		}else if(flag == 'b' || flag == 'B'){
			traverse_list();
		}

    }
}
