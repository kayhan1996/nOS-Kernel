#include "process.h"
#include "timer.h"
#include "interrupts.h"
#include "stdbool.h"
#include "mm.h"

extern void switch_context(struct Process *prev, struct Process *next);
extern void new_process(struct Process *process);

struct Process *current;
struct Process *GlobalProcessTable[32];
int numProcesses;


void init_processes(void){
    enable_irq();
	enable_interrupt_controller();
	init_arm_timer(TIME_SLICE);

    struct Process *kernelMain = (struct Process*) allocate_page_frame;

    kernelMain->priority = High;
    kernelMain->state = Running;

    GlobalProcessTable[0] = kernelMain;

    current = kernelMain;
    numProcesses = 1;
}

void create_process(uint64_t* args, uint64_t* function){
    struct Process *p = (struct Process*) allocate_page_frame();
    p->state = Ready;
    p->priority = High;
    p->context.args = args;
    p->context.sp = p+4096;
    p->context.elr = function;
    p->pid = ++numProcesses;
    GlobalProcessTable[numProcesses] = p;
}

void switch_to(struct Process *process){
    if(current == process) return; 
    switch_context(current, process);
    current = process;
}

void schedule(){
    if(--current->counter > 0 || current->preempt > 0){
        return;
    }
    current->counter = 0;

    int next, maxCount;
    struct Process *process;

    while(true){
        next = 0;
        maxCount = -1;

        for(int i = 0; i < numProcesses; i++){
            process = &GlobalProcessTable[i];
            if(process != 0 && process->state == Ready && process->counter > maxCount){
                maxCount = process->counter;
                next = i;
            }
        }

        if(maxCount != 0){
            break;
        }

        for(int i = 0; i < numProcesses; i++){
            process = &GlobalProcessTable[i];
            if(process != 0){
                process->counter = (process->counter >> 1) + process->priority;     //priority <= process->counter <= 2*priority
            }
        }
    }

    switch_to(&GlobalProcessTable[next]);
}