.section ".text.boot"

.global _start

_start:
    //definitions of the CP15 registers in the core-specific Technical Reference Manual (TRM)
    //Accesses MPIDR, "Read Multiprocessor Affinity Register
    mrc p15, #0, r1, c0, c0, #5 //Reads processor number from hidden core register and places it in r1
    and r1, r1, #3  //CPU ID for 4 Processors
    cmp r1, #0      //If CPU ID == 0
    bne halt    //if not core 0, go to halt

    mov sp, #0x8000

    ldr r4, =__bss_start
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    ldr r9, =__bss_end
    bne     2f  //branch if not equal label 2 in the forward, can also use .2

1:
    //stores 4 x 8 bit values from r5-r8 = 0x0000 into the location at r4
    //effectively zeros out the bss
    stmia r4!, {r5-r8}  //store multiple Increment After
2:
    cmp r4, r9
    blo 1b      //branch if not equal label 1 in the backwards

    ldr r3, =kernel_main
    blx r3      //after return, go to halt

halt:   //infinite loop where the cores 1-3 wait for event
    wfe
    b halt