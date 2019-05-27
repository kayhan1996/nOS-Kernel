.globl switch_context
//void switch_context(struct Process *prev, struct Process *next);
switch_context:


.globl  new_process
new_process:
    mov     x19, x0
    ldr     x0, [x19, #0]
    ldr     x1, [x19, #8]
    ldr     x2, 
    blx     x1