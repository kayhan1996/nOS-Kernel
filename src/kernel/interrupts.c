#include "interrupts.h"

#define IRQ_BASE    0x3f00b200
#define System_Timer       (1 << 1)

struct IRQ_PERIPHERAL {
    volatile uint32_t BASIC_PENDING;
    volatile uint32_t PENDING_1;
    volatile uint32_t PENDING_2;
    volatile uint32_t FIQ_CONTROL;
    volatile uint32_t ENABLE_IRQS1;
    volatile uint32_t ENABLE_IRQS2;
    volatile uint32_t ENABLE_BASIC_IRQS;
    volatile uint32_t DISABLE_IRQS1;
    volatile uint32_t DISABLE_IRQS2;
    volatile uint32_t DISABLE_BASIC_IRQS;
};

#define IRQ         ((struct IRQ_PERIPHERAL*) IRQ_BASE)

void enable_interrupt_controller(){
    IRQ->ENABLE_IRQS1 |= System_Timer;
}

uint32_t get_pending_interrupts(){
    return IRQ->PENDING_1;
}