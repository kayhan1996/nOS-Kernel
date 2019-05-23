#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>

#define TIME_SLICE              50000000

typedef uint64_t process_ptr;

enum ProcessState {
    Running,
    Ready,
    Blocked
};

enum ProcessPriority {
    High
};

struct ContextRegisters {
    uint64_t sp;
    uint64_t elr;
    uint64_t spsr;
};

struct Process {
    struct ContextRegisters context;
    enum ProcessState state;
    enum ProcessPriority priority;
    uint64_t counter;
    uint64_t preempt;
};

struct Process GlobalProcessTable[32];
int numProcesses;

struct Process *current;

void init_processes(void);

void schedule();

void create_process(process_ptr *process, uint64_t *args);

#endif