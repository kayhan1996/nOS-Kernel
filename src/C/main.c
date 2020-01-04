
#include "asm.h"
#include "mm.h"
#include "mmu.h"
#include "printx.h"
#include "process.h"
#include "uart.h"

#include "interrupts.h"
#include "timer.h"

#include "pretty_print.h"

#include "framebuffer.h"

#include "mailbox.h"

#include "canvas.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void test_process(char *str) {
  uint64_t currentEL;
  printf("Current EL: %d\n", currentEL);
  while (1) {
    printf(str);
    delay(1000000);
  }
}

void kernel_main() {
  init_uart();
  init_printf(0, putc);
  printf("Kernel Program Started.\n");

  init_framebuffer();
  // testCanvas();

  for (int i = 0; i < 15; i++) {
  	printf("Hello! %d", i);
  }

  init_memory_manager();
  init_process_manager();
  enable_interrupt_controller();
  init_arm_timer(300000000);
  enable_irq();

  create_process(test_process, "abcde\n");
  create_process(test_process, "     		12345\n");

  while (1) {
  }
}