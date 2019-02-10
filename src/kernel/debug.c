#include "debug.h"
#include "mstdio.h"
#include "stddef.h"
#include "uart.h"

static void print_value(uint64_t *address){
    volatile uint64_t value = *address;
    printhex(address); print("  |  "); printhex(value); println("");
}

static void print_list(uint64_t *address, int count){
    print("---------------------------------------------------\n");
    print("                       |     12 |  8 |  4 |  0 |\n");
    for(int i = 0; i < count; i++){
        if(address >= 2){
            print_value(address);
            address -= 2;
        }
    }
}

void print_memory(uint64_t *new_address){

    register uint64_t *sp, *stack;

    asm("mov %[stack_pointer], sp" : [stack_pointer] "=r" (sp) ::);

    if(new_address == NULL){
        println("\n--------------PRINTING STACK-----------------");
        print("Stack Pointer: ");
        printhex(sp);
        println("");
        stack = sp;
    }else{
        print("\n--------------PRINTING MEMORY @"); printhex(new_address); println("-----------------");
        stack = new_address;
    }
    stack -= 2;
    uint64_t value = *stack;    
    char flag = 0x41;

    print("(q to Quit), (s to Search), (r to Return to Stack Pointer) (UP), (DOWN)\n\n");
    print("        Address        |           Value\n");
    do{
        

        

        if(flag == 0x41){
            stack += 2 * 4;
            print_list(stack, 4);
        }else if(flag == 0x42){
            stack -= 2 * 4;
            print_list(stack, 4);
        }else if(flag == 's'){
            print("---------------------------------------------------\n");
            print("Search for Address: ");
            stack = gethex();
            print_list(stack, 4);
        }else if(flag == 'r'){
            print("---------------------------------------------------\n");
            print("Return to Stack Pointer @");
            printhex(sp);
            println("");
            stack = sp;
            print_list(stack, 4);
        }

        flag = uart_getc();
    }while(flag != 'q');


    println("-----------------END MEMORY-------------------");
}