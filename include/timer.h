#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

void init_timer();

void init_arm_timer(int IRQ_interval);

void set_next_time();

void set_next_time_arm();

uint32_t get_current_time();

uint64_t get_current_time_arm();

uint32_t get_next_time();

void timer_delay(uint32_t seconds);

void timer_delay_arm(uint64_t msecs);

#endif