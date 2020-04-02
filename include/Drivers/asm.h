#ifndef ASM_H
#define ASM_H

extern void mmio_write ( unsigned long, unsigned int ) __asm__("mmio_write");
extern unsigned int mmio_read ( unsigned long ) __asm__("mmio_read");

#endif  /*ASM_H*/