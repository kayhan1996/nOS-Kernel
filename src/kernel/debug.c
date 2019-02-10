#include "debug.h"
#include "mstdio.h"
#include "stdint.h"
#include "uart.h"

static print_value(uint64_t *address){
    volatile uint64_t value = *address;
    printhex(address); print("  |  "); printhex(value); println("");
}

static void print_list(uint64_t *address, int count){
    print("---------------------------------------------------\n");
    print("                       |     12 |  8 |  4 |  0 |\n");
    for(int i = 0; i < count; i++){
        print_value(address);
        address -= 2;
    }
}

void print_memory(){
    register uint64_t *sp = 0;

    
    asm("mov %[stack_pointer], sp" : [stack_pointer] "=r" (sp) ::);
    println("\n--------------PRINTING STACK-----------------");

    print("Stack Pointer: ");
    printhex(sp);
    println("");
    uint64_t *stack = sp;
    uint64_t value = *stack;
    char flag;

    print("(q to Quit), (s to Search), (r to Return to Stack Pointer) (UP), (DOWN)\n\n");
    print("        Address        |           Value\n");

    do{
        

        flag = uart_getc();

        if(flag == 0x41){
            stack += 2 * 4;
            print_list(stack, 4);
        }else if(flag == 0x42){
            stack -= 2 * 4;
            print_list(stack, 4);
        }else if(flag == 's'){
            print("Search for address: ");
            stack = gethex();
            print_list(stack, 4);
        }else if(flag == 'r'){
            print("Return to stack pointer\n");
            stack = sp;
            print_list(stack, 4);
        }

        
        
    }while(flag != 'q');


    println("-----------------END STACK-------------------");
}