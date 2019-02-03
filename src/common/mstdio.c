#include "mstdio.h"
#include "uart.h"

void println(const char *str){
    uart_puts(str);
    uart_putc('\n');
}

void print(const char *str){
    uart_puts(str);
}

void printhex(unsigned int number){
    unsigned int n;
    for(int i = 28; i >= 0; i -= 4){
        n = (number >> i);
        n &= 0xF;
        n += (n > 9) ? 0x37 : 0x30;
        uart_putc(n);
    }
}