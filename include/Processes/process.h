#ifndef __PROCESS_H__
#define __PROCESS_H__

#define PR_OFFSET 0xa00000
#define CURRENT_PROCESS PR_OFFSET
#define PROCESS_TABLE_BASE (PR_OFFSET + 64)

#ifdef __ASSEMBLER__    //Assembly only

#else   //C only
#include <stdint.h>

#define MAX_PROCESSES           32

enum PSTATES { User = 0x0, System = 0x3c4 };

struct Context{
    uint64_t sp;
    uint64_t PSTATE;
};

struct Process {
    struct Context context;
    uint64_t *func;
    uint64_t *args;
    uint64_t pid;
    uint64_t count;
};


void init_process_manager();
void create_process(void *fn, void *args);
void schedule();

#endif // __ASSEMBLER__

#endif // __PROCESS_H__
