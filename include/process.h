#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>

enum ProcessState {
    Running,
    Ready,
    Blocked
};

enum ProcessPriority {
    High
};

struct ContextRegisters {
    uint64_t x19;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;
    uint64_t sp;
    uint64_t lr;
};

struct Task {
    enum ProcessState state;
    enum ProcessPriority priority;
    struct ContextRegisters context;
    uint64_t counter;
    uint64_t preempt;
};

#endif