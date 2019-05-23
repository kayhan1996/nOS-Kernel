#include "process.h"
#include "timer.h"
#include "interrupts.h"
#include "stdbool.h"

extern void switch_context(struct Process *prev, struct Process *next);
extern void new_process(process_ptr *process, void *args);

void init_processes(void){
    enable_irq();
	enable_interrupt_controller();
	init_arm_timer(TIME_SLICE);

    GlobalProcessTable[0] = (struct Process){
        .context = {0, 0, 0},
        .state = Running,
        .priority = High,
        .counter = 0,
        .preempt = 0
    };
    numProcesses++;

    current = &GlobalProcessTable[0];
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
    enable_irq();

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

void create_process(process_ptr *process, uint64_t *args){
    struct Process *p = &GlobalProcessTable[++numProcesses];
    p->context.elr = (uint64_t)new_process;
}