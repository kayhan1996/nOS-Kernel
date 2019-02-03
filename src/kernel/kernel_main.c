#include "uart.h"
#include "mstdio.h"
#include "mailbox.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(unsigned long r0, unsigned long r1, unsigned long atags)
{
	uart_init();
	println("UART initiated");
	println("Kernel Program Started");

	mailbox[0] = 8*4;
	mailbox[1] = MBOX_REQUEST;
	mailbox[2] = MBOX_TAG_GETSERIAL;

	mailbox[3] = 8;
	mailbox[4] = 8;
	mailbox[5] = 0;
	mailbox[6] = 0;

	mailbox[7] = MBOX_TAG_LAST;

	print("Address of mailbox: ");
	printhex(mailbox);

	if(callMailBox(MBOX_CH_PROP)){
		println("Success");
	}else{
		println("Fail");
	}

	while (1) {
		uart_putc(uart_getc());
        uart_putc('\n');
    }
}
