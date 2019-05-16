#include "timer.h"

#define TIMER_BASE          0x3f003000
#define TIMER_M1            (1 << 1)
struct SYSTEM_TIMER {
    volatile uint32_t CS;
    volatile uint32_t CLO;
    volatile uint32_t CHI;
    volatile uint32_t C0;
    volatile uint32_t C1;
    volatile uint32_t C2;
    volatile uint32_t C3;
};

#define TIMER       ((struct SYSTEM_TIMER*) TIMER_BASE)

uint32_t next_time;
const uint32_t interval = 4000000;

void init_timer(){
    next_time = TIMER->CLO;
    next_time += interval;
    TIMER->C1 = next_time;
}

void set_next_time(){
    next_time += interval;
    TIMER->C1 = next_time;
    TIMER->CS = TIMER_M1;
}

uint32_t get_current_time(){
    return TIMER->CLO;
}

uint32_t get_next_time(){
    return TIMER->C1;
}

void timer_delay(uint32_t seconds){
    int t = TIMER->CLO;
    int dt = t + (seconds * 1000000);
    while(TIMER->CLO != dt){
        asm("nop");
    }
}