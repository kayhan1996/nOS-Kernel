.section ".text.boot"

.globl _start

_start:
    mrs x0, mpidr_el1   //retrieves core registers info on processor affinity
    and x0, x0, #3      //Checks if not processor 0
    cbz x0, 1f          //branch to 1
hang:
    b hang

1:
    adr x0, __bss_start //Defined in linker.ld
    adr x1, __bss_end   //Defined in linker.ld
    sub x1, x1, x0
    bl  memzero

    ldr x1, =__start

    msr     sp_el1, x1
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
    // change execution level to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, 5f
    msr     elr_el2, x2
    eret
5:
    mov sp, x1

    bl  kernel_main     //jump to main function
    b   hang            //main should never return


memzero:
    str wzr, [x0], #8   //zero out address at x0, then increment x0 by 8 bytes
    b.gt memzero
    ret
    

