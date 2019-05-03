#ifndef ASM_H
#define ASM_H

extern void delay ( unsigned long) __asm__("delay");
extern void mmio_write ( unsigned long, unsigned int ) __asm__("mmio_write");
extern unsigned int mmio_read ( unsigned long ) __asm__("mmio_read");

extern void LED_ACT_ON(void) __asm__("LED_ACT_ON");
extern void LED_ACT_OFF(void) __asm__("LED_ACT_OFF");
#endif  /*ASM_H*/