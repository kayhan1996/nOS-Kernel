.macro  entry   label
.align  7
    b   \label
.endm

.macro  save_registers
    //temporarily save register to use for PCB location
    str     x30, [sp]

    //load location of current Process Control Block
    ldr     x30, =0xa00000
    ldr     x30, [x30]

    //store registers to PCB
    stp     x0, x1, [x30, #16 * 0]
    stp     x2, x3, [x30, #16 * 1]
    stp     x4, x5, [x30, #16 * 2]
    stp     x6, x7, [x30, #16 * 3]
    stp     x8, x9, [x30, #16 * 4]
    stp     x10, x11, [x30, #16 * 5]
    stp     x12, x13, [x30, #16 * 6]
    stp     x14, x15, [x30, #16 * 7]
    stp     x16, x17, [x30, #16 * 8]
    stp     x18, x19, [x30, #16 * 9]
    stp     x20, x21, [x30, #16 * 10]
    stp     x22, x23, [x30, #16 * 11]
    stp     x24, x25, [x30, #16 * 12]
    stp     x26, x27, [x30, #16 * 13]
    stp     x28, x29, [x30, #16 * 14]

    //restore x30 register and save special registers to PCB
    mov     x0, x30

    ldr     x30, [sp]

    mrs     x1, ELR_EL1
    mrs     x2, SPSR_EL1
    mrs     x3, SP_EL0
    
    stp     x30, x1, [x0,  #16 * 15]
    stp     x2,  x3, [x0, #16 * 16]
.endm

.macro  load_registers
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
.endm

.align   11
.globl  vectors
vectors:
    entry   exc_handle
    entry   handle_irq
    entry   exc_handle
    entry   exc_handle

    entry   exc_handle
    entry   exc_handle
    entry   exc_handle
    entry   exc_handle

    entry   exc_handle
    entry   handle_irq
    entry   exc_handle
    entry   exc_handle

    entry   exc_handle
    entry   exc_handle
    entry   exc_handle
    entry   exc_handle
    

.globl  exc_handle
exc_handle:
    b  error_handler

handle_irq:
    ldr x10, =0x7777
    
    //temporarily save register to use for PCB location
    str     x30, [sp]

    //load location of current Process Control Block
    ldr     x30, =0xa00000
    ldr     x30, [x30]

    //store registers to PCB
    stp     x0, x1, [x30, #16 * 0]
    stp     x2, x3, [x30, #16 * 1]
    stp     x4, x5, [x30, #16 * 2]
    stp     x6, x7, [x30, #16 * 3]
    stp     x8, x9, [x30, #16 * 4]
    stp     x10, x11, [x30, #16 * 5]
    stp     x12, x13, [x30, #16 * 6]
    stp     x14, x15, [x30, #16 * 7]
    stp     x16, x17, [x30, #16 * 8]
    stp     x18, x19, [x30, #16 * 9]
    stp     x20, x21, [x30, #16 * 10]
    stp     x22, x23, [x30, #16 * 11]
    stp     x24, x25, [x30, #16 * 12]
    stp     x26, x27, [x30, #16 * 13]
    stp     x28, x29, [x30, #16 * 14]

    //restore x30 register and save special registers to PCB
    mov     x0, x30

    ldr     x30, [sp]

    mrs     x1, ELR_EL1
    mrs     x2, SPSR_EL1
    mrs     x3, SP_EL0
    
    stp     x30, x1, [x0,  #16 * 15]
    stp     x2,  x3, [x0, #16 * 16]

    bl  exception_handler
    load_registers

