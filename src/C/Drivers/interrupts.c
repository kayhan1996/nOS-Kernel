#include "Drivers/interrupts.h"

#define CORE_TIMER_IRQ      ((volatile uint32_t*)0x40000040)
#define IRQ_BASE            0x3f00b200
#define System_Timer        (1 << 1)
#define ROUTE_IRQ_TO_CORE0  (1 << 1)

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
    *CORE_TIMER_IRQ |= ROUTE_IRQ_TO_CORE0;
}

uint32_t get_pending_interrupts(){
    return IRQ->PENDING_1;
}

void enable_irq(){
    asm("msr daifclr, #2");
}

void disable_irq(){
    asm("msr daifset, #2");
}