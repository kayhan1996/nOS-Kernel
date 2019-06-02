#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdint.h>

#define MAX_PROCESSES           32

struct Context{
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;
    uint64_t lr;
    uint64_t ELR_EL1;
    uint64_t SPSR_EL1;
    uint64_t sp;
};

struct Process{
    struct Context context;
    uint64_t *func;
    uint64_t *args;
    uint64_t pid;
    uint64_t count;
};


void init_process_manager();
void create_process(uint64_t *fn, uint64_t *args);
void schedule();

#endif // __PROCESS_H__
