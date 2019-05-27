.globl switch_context
switch_context:
    mrs     x2, SP_EL0
    mrs     x3, ELR_EL1
    mrs     x4, SPSR_EL1
    str     x2, [x0], #8
    str     x3, [x0], #8
    str     x4, [x0], #8

    ldr     x2, [x1], #8
    ldr     x3, [x1], #8
    ldr     x4, [x1], #8
    msr     SP_EL0, x2
    msr     ELR_EL1, x3
    msr     SPSR_EL1, x4
    ret

.globl  new_process
new_process:
    mov     x3, x0
    mov     x0, x1
    blr     x3