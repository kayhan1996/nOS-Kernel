#include "printx.h"
#include "stdint.h"
#include "timer.h"

enum Exception_State{
    Synchronouse = 0,
    IRQ = 1,
    FIQ = 2,
    SError = 3
};

void exception_handler(enum Exception_State exception_state){
    switch(exception_state){
        case IRQ:
            set_next_time();
            printf("IRQ interrupt occurred\n");
            break;
        default:
            printf("Unknown Exception Occurred\n");
    }
}