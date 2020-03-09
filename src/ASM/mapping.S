.macro addPageEntry flags addr table shift
    mov x6, \flags
    mov x7, \addr
    orr x6, x6, x7, lsl #(12+\shift)
    str x6, [\table]
.endm

.globl create_identity_map
create_identity_map:

    table .req x0
    count .req x1
    addr  .req x2
    flag  .req x3

    /*Creating mapping for normal RAM*/
    adr     table, __block_table_start
    mov     count, #(488)
    mov     addr, #0
    mov     flag, #0x711
NORMAL:
    addPageEntry flag addr table 9
    add     table, table, #8
    add     addr, addr, #1
    sub     count, count, #1
    cbnz    count, NORMAL

    /*Create mapping for video memory */
    mov     count, #(16)
    mov     flag, #0x44d
VIDEO:
    addPageEntry flag addr table 9
    add     table, table, #8
    add     addr, addr, #1
    sub     count, count, #1
    cbnz    count, VIDEO

    /*Create mapping for peripherals */
    mov     count, #(8)
    mov     flag, #0x445
PERIPHERALS:
    addPageEntry flag addr table 9
    add     table, table, #8
    add     addr, addr, #1
    sub     count, count, #1
    cbnz    count, PERIPHERALS

    /*Create mapping for mailbox at 1024MB */
    mov     count, #1
    mov     flag, #0x441
MAILBOX:
    addPageEntry flag addr table 9
    add     table, table, #8
    add     addr, addr, #1
    sub     count, count, #1
    cbnz    count, MAILBOX

    /*Set 2nd table to addresses of first level tables*/
    adr     table, __table_table_start
    adr     addr, __block_table_start
    lsr     addr, addr, #12
    mov     flag, #0x3

    //T2[0] = &Blocks[511-0]
    addPageEntry flag addr table 0
    
    //T[1] = &Blocks[512]
    add     table, table, #8
    add     addr, addr, #0x1
    addPageEntry flag addr table 0

    /* Setup translation properties */
    dsb     sy
    mov     x0, 0x440c
    mov     x1, 0x0400
    lsl     x0, x0, #16
    orr     x0, x0, x1
    // 0x440c0400
    msr     mair_el1, x0

    adr     table, __table_table_start
    msr     ttbr0_el1, table
    msr     ttbr1_el1, table
    mov     x0, #0xb519
    mov     x1, #0x3519
    lsl     x0, x0, #16
    orr     flag, x0, x1
    msr     tcr_el1, flag
    isb

    /* Enable MMU */
    // 0x30d0081f
    mov     x0, #0x30d0
    mov     x1, #0x081f
    lsl     x0, x0, #16
    orr     x0, x0, x1
    //mrs     x0, sctlr_el1
    //orr     x0, x0, #1
    msr     sctlr_el1, x0
    isb

    /* Return to high memory */
    mov     x0, #0xFFFFFF80
    lsl     x0, x0, #32
    orr     lr, lr, x0

    ret
