#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

void init_timer();

void set_next_time();

uint32_t get_current_time();

uint32_t get_next_time();

void timer_delay(uint32_t seconds);

#endif