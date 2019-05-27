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
    schedule();
    set_next_time_arm();
}

void exception_handler(enum Exception_State exception_state){
    printf("---------------------------------\n");
    switch(exception_state){
        case IRQ:
            timer_tick();
            break;
        default:
            printf("Unknown Exception Occurred\n");
    }
    printf("----------------------------------\n");
}