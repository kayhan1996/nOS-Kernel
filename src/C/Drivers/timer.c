#include "Drivers/timer.h"
#include "Drivers/peripherals.h"
#include "Libraries/printx.h"

#define TIMER_BASE          (PERIPHERAL_BASE + 0x3000)
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
uint32_t arm_interval;

void init_timer(){
    next_time = TIMER->CLO;
    next_time += interval;
    TIMER->C1 = next_time;
}

void init_arm_timer(int IRQ_interal){
    arm_interval = IRQ_interal;
    register uint64_t enable = 0b1;
    asm("msr CNTP_CTL_EL0, %[enable]" :: [enable] "r" (enable));
    register uint64_t countdown = 100000000;
    asm("msr CNTP_TVAL_EL0, %[countdown]" :: [countdown] "r" (countdown));
}

void set_next_time(){
    next_time += interval;
    TIMER->C1 = next_time;
    TIMER->CS = TIMER_M1;
}

void set_next_time_arm(){
    register tmp = arm_interval;
	asm("msr CNTP_TVAL_EL0, %[tmp]" :: [tmp] "r" (tmp));
}

uint32_t get_current_time(){
    return TIMER->CLO;
}

uint64_t get_current_time_arm(){
    uint64_t time;
    asm("mrs %[time], CNTPCT_EL0" : [time] "=r" (time));
    return time;
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

void timer_delay_arm(uint64_t msecs){
    int t = get_current_time_arm();
    int end = t+msecs*1000;

    while(t < end){
        t = get_current_time_arm();
    }
}

void system_timer(uint32_t microseconds){
    volatile uint32_t t = *((uint32_t*)(TIMER_BASE+0x4));
    uint32_t end = t+microseconds;
    while(t < end){
        t = *((uint32_t*)(TIMER_BASE+0x4));
    }
}

void delay(uint32_t microseconds){
    system_timer(microseconds);
}

void delay_ms(uint32_t ms){
    uint32_t cycles = 1000*ms;
    system_timer(cycles);
}