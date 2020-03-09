#include "Processes/process.h"
#include "Drivers/interrupts.h"
#include "Memory/frame_allocator.h"
#include "Memory/memory_descriptor.h"
#include "Libraries/printx.h"

extern uint64_t __user_start__;
extern uint64_t __user_end__;

extern void load_thread(struct Process *proc, void *func, void *args);
extern void load_core(void *func, void *addr, uint64_t bytes);
extern void init_process(void);
extern uint64_t __table_table_start;

struct Process **ProcessTable;
struct Process **Current;

int numProcesses;

void init_process_manager(){
    ProcessTable = (struct Process*)KERNEL_ADDRESS(PROCESS_TABLE_BASE);
    Current = (struct Process *)KERNEL_ADDRESS(CURRENT_PROCESS);

    struct Process *p = (struct Process*) allocate_page_frames(0)->address;
    p->context.PSTATE = System;
    p->pid = 0;
    p->count = 100;
    p->memory = PHYSICAL_ADDRESS(&__table_table_start);

    ProcessTable[0] = p;
    Current[0] = p;

    numProcesses = 1;
}

void create_process(void *func, void *args){
    disable_irq();
    struct Process *p = allocate_page_frames(0)->address;
    p->context.PSTATE = User;
    ProcessTable[numProcesses] = p;
    p->pid = numProcesses;
    numProcesses += 1;
    
    Memory_Descriptor md = create_process_memory_descriptor();
    p->memory = PHYSICAL_ADDRESS(create_new_table(&md));

    p->context.sp = md.stack.end - 272; //(uint64_t)p + 4096 - 272;

    load_thread(p, 0x0, args);
    load_core(func, 0x0, &__user_end__ - &__user_start__);

    enable_irq();
}

void switch_to(struct Process *new){
    if(Current[0] == new) return;
    struct Process *old = Current[0];
    Current[0] = new;

    uint64_t Table = new->memory;

    asm volatile("msr ttbr0_el1, %0" ::"r"(Table));
    asm volatile("tlbi vmalle1is");
    asm volatile("dsb ish" ::: "memory");

    printf("\nProcess ID: %d\n", Current[0]->pid);
    printf("Process is in %s mode\n", (new->context.PSTATE == User) ? "User" : "System");
    //printf("Switching Memory context to table: 0x%x\n", memorymap);
}

void schedule(){
    static int n = 0;
    n = (n+1) % numProcesses;
    switch_to(ProcessTable[n]);
}