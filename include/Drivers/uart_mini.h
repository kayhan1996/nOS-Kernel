#ifndef MINI_UART_H
#define MINI_UART_H

#include "stdint.h"

void init_mini_uart();

void mu_send(char c);
char mu_receive();

#endif