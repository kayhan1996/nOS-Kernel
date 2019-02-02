#include "mstdio.h"
#include "uart.h"

void println(const char *str){
    uart_puts(str);
}