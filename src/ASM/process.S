//void load_core(void *func, void *addr, uint64_t bytes);
.globl load_core
load_core:
    ldr     w3, [x0], +4
    str     x3, [x1], +4
    subs    x2, x2, #4
    cbnz    x2, load_core
    ret

//void load_thread(struct Process *proc, void *func, void *args);
.globl load_thread
load_thread:
    proc .req x0
    func .req x1
    args .req x2
    stack .req x3
    state .req x4
    
    //load address of thread stack pointer
    ldr     stack, [proc, #0]

    //load process state PSTATE
    ldr     state, [proc, #8]

    //move thread context to stack to load at next context switch
    str     args, [stack, #0]
    stp     func, state, [stack, #16*16]   //save function to ELR and state to SPSR registers on return

    //TODO: set link register (x30) to an thread exit function
    ret