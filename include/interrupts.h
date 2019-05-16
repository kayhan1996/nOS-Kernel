#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "stdint.h"

void enable_interrupt_controller();

uint32_t get_pending_interrupts();

#endif