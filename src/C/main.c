
#include "Memory/mm.h"
#include "Drivers/interrupts.h"
#include "Libraries/printx.h"
#include "Processes/process.h"
#include "Drivers/timer.h"
#include "Drivers/uart.h"

#include "Memory/kmalloc.h"

/* Use C linkage. */
#if defined(__cplusplus)
extern "C" void test_process(char *str);
extern "C" void kernel_main();
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

  init_memory();

  int *z = kmalloc(10 * sizeof(int), 0);

  for (int i = 0; i < 10; i++) {
    z[i] = i << 12;
  }

  int *x = kmalloc(100 * sizeof(int), 0);

  for (int i = 0; i < 100; i++) {
    x[i] = i << 5;
  }

  kfree(z);
  kfree(x);

  // init_process_manager();

  // enable_interrupt_controller();
  // init_arm_timer(3000000);
  // enable_irq();

  // create_process(test_process, "abcde\n");
  // create_process(test_process, "     		12345\n");

  // while (1) {
  // }
}