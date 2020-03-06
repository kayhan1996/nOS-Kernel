#include "boot.h"

.section ".text.boot"

.globl _start

_start:
    mrs     x0, mpidr_el1   //retrieves core registers info on processor affinity
    and     x0, x0, #0b11   //Extract bits [1:0]

    cmp     x0, #0          //if core 0
    b.eq    _start_core0

    cmp     x0, #1
    b.eq    _start_core1
hang:
    b hang

_start_core0:
    //Setup C environment prereqs

    //zero out BSS section
    adr     x0, __bss_start //Defined in linker.ld
    adr     x1, __bss_end   //Defined in linker.ld
    sub     x1, x1, x0
    bl  memzero

    //set stack pointers
    mov     x1, INIT_SP_EL0
    msr     SP_EL0, x1
    //seperate stack pointer for exceptions in EL1
    mov     x1, INIT_SP
    msr     SP_EL1, x1 

    //Setup Hardware
    
    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
    //enable FPU
    mov     x0, #0x33FF
    msr     cptr_el2, x0
    msr     hstr_el2, xzr
    mov     x0, #(3 << 20)
    msr     cpacr_el1, x0
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // Setup SCTLR access
    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2
    // Setup exception table
    adr     x2, vectors
    msr     vbar_el1, x2
    // change execution level to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, C_Kernel
    msr     elr_el2, x2
    eret
    
C_Kernel:   
    msr     SPSel, #1
    bl      kernel_main     //jump to main function
    b       hang            //main should never return


_start_core1:
    adr     x1, #0x25000
    ldr     x0, [x0]
    cmp     x0, #0x36
    b.ne   _start_core1

    adr     x1, #0x22000
    msr     SP_EL1, x1 
    msr     SPSel, #1


memzero:
	str     xzr, [x0], #8
	subs    x1, x1, #8
	b.gt    memzero
	ret
