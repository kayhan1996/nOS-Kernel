#include "Processes/process.h"
#include "Drivers/interrupts.h"
#include "Memory/frame_allocator.h"
#include "Memory/memory_descriptor.h"
#include "Libraries/printx.h"

extern uint64_t __USER_START__;
extern uint64_t __USER_END__;

extern void load_thread(struct Process *proc, void *func, void *args);
extern void load_core(void *func, void *addr, uint64_t bytes);
extern void init_process(void);

struct Process **ProcessTable;
struct Process **Current;

int numProcesses;

void init_process_manager(){
    ProcessTable = (struct Process*)PROCESS_TABLE_BASE;
    Current = (struct Process *)CURRENT_PROCESS;

    struct Process *p = (struct Process*) allocate_page_frames(0)->address;
    p->context.PSTATE = System;
    p->pid = 0;
    p->count = 100;

    ProcessTable[0] = p;
    Current[0] = p;

    numProcesses = 1;
}

void create_process(void *func, void *args){
    disable_irq();
    struct Process *p = allocate_page_frames(0)->address;
    p->context.sp = 0x1004000; //(uint64_t)p + 4096 - 272;
    p->context.PSTATE = User;
    ProcessTable[numProcesses] = p;
    numProcesses += 1;

    Memory_Descriptor md = create_process_memory_descriptor();
    Table_Descriptor *process_table = create_new_table(&md);

    uint64_t size = (uint64_t)&__USER_END__ - (uint64_t)&__USER_START__;
    load_thread(p, func, args);
    //load_core(func, 0x200000, size);

    enable_irq();
}

void switch_to(struct Process *new){
    if(Current[0] == new) return;
    struct Process *old = Current[0];
    Current[0] = new;
    //enable_irq();
    //switch_context(old, new);
}

void schedule(){
    // printf("LOC: 0x%x\n", loc);
    // printf("SPSR: 0x%x\n", Current->context.SPSR_EL1);
    // printf("ELR: 0x%x\n", Current->context.ELR_EL1);
    // printf("SP: 0x%x\n", Current->context.sp);
    static int n = 0;
    n = (n+1) % numProcesses;
    switch_to(ProcessTable[n]);
}