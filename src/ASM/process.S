
.globl switch_context
switch_context:
    // load address of current PCB
    ldr     x30, =0xa00000
    ldr     x30, [x30]

    // load special registers 
    ldp     x0, x1, [x30, #16 * 15]
    ldp     x2, x3, [x30, #16 * 16]

    msr     ELR_EL1, x1
    msr     SPSR_EL1, x2
    msr     SP_EL0, x3

    // load registers
    ldp     x0, x1, [x30, #16 * 0]
    ldp     x2, x3, [x30, #16 * 1]
    ldp     x4, x5, [x30, #16 * 2]
    ldp     x6, x7, [x30, #16 * 3]
    ldp     x8, x9, [x30, #16 * 4]
    ldp     x10, x11, [x30, #16 * 5]
    ldp     x12, x13, [x30, #16 * 6]
    ldp     x14, x15, [x30, #16 * 7]
    ldp     x16, x17, [x30, #16 * 8]
    ldp     x18, x19, [x30, #16 * 9]
    ldp     x20, x21, [x30, #16 * 10]
    ldp     x22, x23, [x30, #16 * 11]
    ldp     x24, x25, [x30, #16 * 12]
    ldp     x26, x27, [x30, #16 * 13]
    ldp     x28, x29, [x30, #16 * 14]
    ldr     x30, [x30, #16 * 15]

    eret

.globl init_process
init_process:
    mov     x0, x20
    mov     x30, x19
    ret