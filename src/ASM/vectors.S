#include "Processes/process.h"

.macro  entry   label
.align  7
    b   \label
.endm

.align   11
.globl  vectors
vectors:
    entry   exc_handle
    entry   handle_irq_el1t
    entry   exc_handle
    entry   exc_handle

    entry   exc_handle
    entry   handle_irq_el1h
    entry   exc_handle
    entry   exc_handle

    entry   exc_handle
    entry   handle_irq_el0
    entry   exc_handle
    entry   exc_handle

    entry   exc_handle
    entry   exc_handle
    entry   exc_handle
    entry   exc_handle


.macro  save_registers, el

    .if \el==0
        msr     SPSel, #0
    .else
        msr     SPSel, #1
    .endif

    sub     sp, sp, #(272)

    //Save sp to current process control block context
    stp     x0, x1, [sp, #-16]!     //Save temp
    mov     x0, #CURRENT_PROCESS    //load Current
    mov     x1, #0xFFFFFF80
    lsl     x1, x1, #32
    orr     x0, x0, x1
    ldr     x0, [x0]                //Current[0]
    mov     x1, sp
    add     x1, x1, 16              //account for temps
    str     x1, [x0, #0]            //Current[0]->context.sp = sp
    ldp     x0, x1, [sp], #16     //Restore temp

    //store registers to PCB
    stp     x0, x1, [sp, #16 * 0]
    stp     x2, x3, [sp, #16 * 1]
    stp     x4, x5, [sp, #16 * 2]
    stp     x6, x7, [sp, #16 * 3]
    stp     x8, x9, [sp, #16 * 4]
    stp     x10, x11, [sp, #16 * 5]
    stp     x12, x13, [sp, #16 * 6]
    stp     x14, x15, [sp, #16 * 7]
    stp     x16, x17, [sp, #16 * 8]
    stp     x18, x19, [sp, #16 * 9]
    stp     x20, x21, [sp, #16 * 10]
    stp     x22, x23, [sp, #16 * 11]
    stp     x24, x25, [sp, #16 * 12]
    stp     x26, x27, [sp, #16 * 13]
    stp     x28, x29, [sp, #16 * 14]
    str     x30, [sp, #16*15]

    mrs     x0, ELR_EL1
    mrs     x1, SPSR_EL1
    
    stp     x0,  x1, [sp, #16 * 16]
.endm

.macro  load_registers
    //load current process
    mov     x0, #CURRENT_PROCESS    //load Current
    mov     x1, #0xFFFFFF80
    lsl     x1, x1, #32
    orr     x0, x0, x1
    ldr     x0, [x0]                //Current[0]
    
    //check if process is user or system and set appropriate SP
    ldr     x1, [x0, #8]            //Current[0]->context.PSTATE
    cmp     x1, #0x3c4
    b.eq    1f
    msr     SPSel, #0
    b       2f
1:
    msr     SPSel, #1
2:
    ldr     x1, [x0, #0]            
    mov     sp, x1                  //$sp = Current[0]->context.sp

    //ldr     x1, [x0, (8*6)]
    //msr     TTBR0_EL1, x1
    //isb

    //load registers from thread stack
    ldp     x0,  x1, [sp, #16 * 16]
    msr     ELR_EL1, x0
    msr     SPSR_EL1, x1

    ldp     x0, x1, [sp, #16 * 0]
    ldp     x2, x3, [sp, #16 * 1]
    ldp     x4, x5, [sp, #16 * 2]
    ldp     x6, x7, [sp, #16 * 3]
    ldp     x8, x9, [sp, #16 * 4]
    ldp     x10, x11, [sp, #16 * 5]
    ldp     x12, x13, [sp, #16 * 6]
    ldp     x14, x15, [sp, #16 * 7]
    ldp     x16, x17, [sp, #16 * 8]
    ldp     x18, x19, [sp, #16 * 9]
    ldp     x20, x21, [sp, #16 * 10]
    ldp     x22, x23, [sp, #16 * 11]
    ldp     x24, x25, [sp, #16 * 12]
    ldp     x26, x27, [sp, #16 * 13]
    ldp     x28, x29, [sp, #16 * 14]
    ldr     x30, [sp, #16 * 15]

    add     sp, sp, #(272)
.endm

.globl  exc_handle
exc_handle:
    mrs x0, FAR_EL1
    mrs x1, ESR_EL1
    mrs x2, ELR_EL1
    b  error_handler

handle_irq_el0:
    save_registers 0
    msr SPSel, #1
    bl  exception_handler
    load_registers
    msr daifclr, #2
    eret

handle_irq_el1t:
    save_registers 1
    bl  exception_handler
    load_registers
    msr daifclr, #2
    eret

handle_irq_el1h:
    save_registers 1
    bl  exception_handler    
    load_registers
    msr daifclr, #2
    eret