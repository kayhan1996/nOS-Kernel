#include "process.h"
#include "interrupts.h"
#include "mm.h"
#include "printx.h"

#define PR_OFFSET                       0xa00000
#define PROCESS_TABLE_BASE              (PR_OFFSET + 16)

struct ProcessRegisters {
    volatile struct Process *Current;
};

#define PR        ((struct ProcessRegisters*)PR_OFFSET)

extern void switch_context(struct Process *old, struct Process *new);
extern void init_process(void);

struct Process **ProcessTable;

int numProcesses;

void init_process_manager(){
    ProcessTable = (struct Process*)PROCESS_TABLE_BASE;

    struct Process *p = (struct Process*) allocate_page_frame();
    p->pid = 0;
    p->count = 100;

    ProcessTable[0] = p;
    PR->Current = p;

    numProcesses = 1;
}

void create_process(uint64_t *func, uint64_t *args){
    struct Process *p = allocate_page_frame();
    p->context.sp = p + 4096;
    p->context.fp = p + 4096;
    p->context.ELR_EL1 = func;
    p->context.SPSR_EL1 = 0x340;
    p->context.x0 = args;
    ProcessTable[numProcesses] = p;
    numProcesses += 1;
}

void schedule(){
    // printf("LOC: 0x%x\n", loc);
    // printf("SPSR: 0x%x\n", PR->Current->context.SPSR_EL1);
    // printf("ELR: 0x%x\n", PR->Current->context.ELR_EL1);
    // printf("SP: 0x%x\n", PR->Current->context.sp);
    static int n = 0;
    n = (n+1) % numProcesses;
    PR->Current = ProcessTable[n];
}