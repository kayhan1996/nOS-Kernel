
.globl switch_context
switch_context:
    // save registers for old process
    stp     x19, x20, [x0, #16 * 0]
    stp     x21, x22, [x0, #16 * 1]
    stp     x23, x24, [x0, #16 * 2]
    stp     x25, x26, [x0, #16 * 3]
    stp     x27, x28, [x0, #16 * 4]
    stp     x29, x30, [x0, #16 * 5]

    mrs     x10, ELR_EL1
    mrs     x11, SPSR_EL1
    mov     x12, sp

    stp     x10, x11, [x0, #16 * 6]
    str     x12, [x0, #16 * 7]

    // load registers for new process
    ldp     x10, x11, [x1, #16 * 6]
    ldr     x12, [x1, #16 * 7]

    msr     ELR_EL1, x10
    msr     SPSR_EL1, x11
    mov     sp, x12

    ldp     x19, x20, [x1, #16 * 0]
    ldp     x21, x22, [x1, #16 * 1]
    ldp     x23, x24, [x1, #16 * 2]
    ldp     x25, x26, [x1, #16 * 3]
    ldp     x27, x28, [x1, #16 * 4]
    ldp     x29, x30, [x1, #16 * 5]

    ret

.globl init_process
init_process:
    mov     x0, x19
    blr     x20