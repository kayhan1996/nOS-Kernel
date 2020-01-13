#include "process.h"
#include "interrupts.h"
#include "Memory/frame_allocator.h"
#include "printx.h"

#define PR_OFFSET                       0xa00000
#define PROCESS_TABLE_BASE              (PR_OFFSET + 16)

extern void switch_context(struct Process *old, struct Process *new);
extern void init_process(void);

struct Process **ProcessTable;
struct Process *Current;
int numProcesses;

void init_process_manager(){
    ProcessTable = (struct Process*)PROCESS_TABLE_BASE;

    struct Process *p = (struct Process*) allocate_page_frames(1)->address;
    p->pid = 0;
    p->count = 100;

    ProcessTable[0] = p;
    Current = p;

    numProcesses = 1;
}

void create_process(uint64_t *func, uint64_t *args){
    struct Process *p = allocate_page_frames(1)->address;
    p->context.sp = p + 4096;
    p->context.SPSR_EL1 = 0x340;
    p->context.x19 = args;
    p->context.x20 = func;
    p->context.lr = init_process;
    ProcessTable[numProcesses] = p;
    numProcesses += 1;
}

void switch_to(struct Process *new){
    if(Current == new) return;
    struct Process *old = Current;
    Current = new;
    enable_irq();
    switch_context(old, new);
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