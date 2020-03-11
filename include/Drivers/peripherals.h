#ifndef PERIPHERALS_H
#define PERIPHERALS_H


#define SYSTEM_BASE     0xFFFFFF8000000000
#define PERIPHERAL_BASE (SYSTEM_BASE + 0x3F000000)
#define GPIO_BASE       (PERIPHERAL_BASE + 0x200000)
#define EMMC_BASE       (PERIPHERAL_BASE + 0x300000)
#define MAILBOX_BASE    (PERIPHERAL_BASE + 0xB880)

#define GPFSEL1         ((volatile uint32_t *)(GPIO_BASE + 0x04))
#define GPPUD           ((volatile uint32_t *)(GPIO_BASE + 0x94))
#define GPPUDCLK0       ((volatile uint32_t *)(GPIO_BASE + 0x98))

#endif