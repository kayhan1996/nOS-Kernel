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

uint64_t gethex(){
    char buffer;
    uint64_t value = 0;

    int i = 0;

    while(i < 16){
        buffer = uart_getc();

        if(buffer == 0xD){
            value = (value >> 4);
            println("");
            break;
        }

        if(buffer >= 'a' && buffer <= 'f'){
            buffer += 'A' - 'a';
        }

        if( (buffer >= '0' && buffer <= '9') || (buffer >= 'A' && buffer <= 'F') ){
            uart_putc(buffer);
            switch (buffer)
            {
                case 'A':
                    value |= 0xA;
                    break;
                case 'B':
                    value |= 0xB;
                    break;
                case 'C':
                    value |= 0xC;
                    break;
                case 'D':
                    value |= 0xD;
                    break;
                case 'E':
                    value |= 0xE;
                    break;
                case 'F':
                    value |= 0xF;
                    break;
                default:
                    value |= (buffer - '0');
                    break;
            }
            value = (value << 4);
            i++;
        }
    }
    println("");
    return value;
}