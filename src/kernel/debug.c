#include "debug.h"
#include "mstdio.h"
#include "stdint.h"
#include "uart.h"


void print_stack(){
    register uint64_t *sp = 0;

    
    asm("mov %[stack_pointer], sp" : [stack_pointer] "=r" (sp) ::);
    println("\n--------------PRINTING STACK-----------------");

    print("Stack Pointer: ");
    printhex(sp);
    println("");
    uint64_t *stack = sp;
    uint64_t value = *stack;
    char flag;

    print("(q for quit), (UP), (DOWN)\n\n");
    print("        Address        |           ");
    print("Value\n");

    do{
        

        flag = uart_getc();

        if(flag == 0x41){
            print("---------------------------------------------------\n");
            print("                       |     12 |  8 |  4 |  0 |\n");
            for(int i = 0; i < 4; i++){
                printhex(stack);
                print("  |  ");
                printhex(value);
                println("");
                stack += 2;
                value = *stack;
            }
        }else if(flag == 0x42){
            print("---------------------------------------------------\n");
            print("                       |     12 |  8 |  4 |  0 |\n");
            for(int i = 0; i < 4; i++){
                stack;
                printhex(stack);
                print("  |  ");
                printhex(value);
                println("");
                stack -= 2;
                value = *stack;
            }
        }

        
        
    }while(flag != 'q');


    println("-----------------END STACK-------------------");
}