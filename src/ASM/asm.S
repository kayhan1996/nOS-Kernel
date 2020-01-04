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
