#include "mstdio.h"
#include "uart.h"
#include "stdint.h"

void println(const char *str){
    uart_puts(str);
    uart_putc('\n');
}

void print(const char *str){
    uart_puts(str);
}

int pow(int b, int e){
    int result = b;
    for(int i = 1; i < e; i++){
        result *= b;
    }
    return result;
}

void printhex(uint64_t number){
    unsigned int n;
    uart_puts("0x");
    for(int i = 60; i >= 0; i -= 4){
        if((i+4) % 16 == 0 && i != 60) print(" ");
        n = (number >> i);
        n &= 0xF;
        n += (n > 9) ? 0x37 : 0x30;
        uart_putc(n);
    }
}

void printdec(unsigned int number){
    int p, r;
    for(int i = 16; i > 0; i--){
        p = pow(10, i);
        r = number / p;
        uart_putc(r + 0x30);
        number -= p * r;
    }
    uart_putc(number + 0x30);
}

