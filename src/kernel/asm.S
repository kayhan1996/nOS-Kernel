.section .text
.global delay
delay:
    subs x0, x0, #1
    bne delay
    ret

.global  mmio_read
mmio_read:
    ldr w0, [x0]
    ret

.global  mmio_write
mmio_write:
    str w1, [x0]
    ret

.globl LED_ACT_ON
LED_ACT_ON:
    .equ BASE,  0x3f200000
    .equ FSEL2, 0x08
    .equ SET0,  0x1c
    .equ CLR0,  0x28
    .equ SELECT_LED29,   0x8000000
    .equ SET_BIT27,  0x20000000

    //Turn on LED
    ldr x0, =BASE
    ldr w2, =SELECT_LED29
    str w2, [x0, #FSEL2]
    ldr w2, =SET_BIT27
    str w2, [x0, #SET0]
    ret

.globl LED_ACT_OFF
LED_ACT_OFF:
    .equ BASE,  0x3f200000
    .equ FSEL2, 0x08
    .equ SET0,  0x1c
    .equ CLR0,  0x28
    .equ SELECT_LED29,   0x8000000
    .equ SET_BIT27,  0x20000000

    //Turn on LED
    ldr x0, =BASE
    ldr w2, =SELECT_LED29
    str w2, [x0, #FSEL2]
    ldr w2, =SET_BIT27
    str w2, [x0, #CLR0]
    ret

