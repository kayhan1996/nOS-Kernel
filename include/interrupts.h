#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "stdint.h"

void enable_interrupt_controller();

uint32_t get_pending_interrupts();

void enable_irq();

void disable_irq();
#endif