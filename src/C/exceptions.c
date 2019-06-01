#include "printx.h"
#include "stdint.h"
#include "timer.h"
#include "process.h"

enum Exception_State{
    Synchronouse = 0,
    IRQ = 1,
    FIQ = 2,
    SError = 3
};

void timer_tick(){
    set_next_time_arm();
    schedule();
}

void exception_handler(){
    printf("Hadoop?\n");
    timer_tick();
}

void error_handler(){
    uint64_t esr = 0;
    asm("mrs    %[esr], esr_el1" : [esr] "=r" (esr));
    printf("ESR: %x\n", esr);
    printf("Unknown Exception Occurred\n");
    while(1){};
}