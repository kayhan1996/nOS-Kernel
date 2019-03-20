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
	
	uint64_t system_control;
	
	init_mmu();
	init_uart();
	init_memory();
	println("MMU Enabled");
	uint64_t *va = map_physical_to_virtual(0);

	// uint64_t t = 0xdeadca7;
	// uint64_t *hadoop = 0xFFFFFF8000000000 | (uint64_t)&t;
	// printhex(*hadoop);
	
	uint64_t t = 0xdeadca7;
	uint64_t *hadoop = 0xFFFFFF8000000000; //| (uint64_t)&t;
	printhex(*hadoop);

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
