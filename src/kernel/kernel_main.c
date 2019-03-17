#include "uart.h"
#include "mstdio.h"
#include "mailbox.h"
#include "mem.h"
#include "asm.h"
#include "debug.h"
#include "mmu.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void access_memory_register(){
	uint64_t memory_register;
	asm volatile("mrs x0, ID_AA64MMFR0_EL1\n\t"
							 "mov %[memr], x0"
							 : [memr] "=r" (memory_register));
	print("Memory Register: ");
	printhex(memory_register);
}

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	
	init_uart();
	init_memory();
	println("MMU Enabled");
	init_mmu();

	int hadoop = 64;
	asm volatile("mrs %0, CurrentEL" : "=r"(hadoop));
	hadoop = hadoop >> 2;
	printhex(hadoop);

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
