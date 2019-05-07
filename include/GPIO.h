#ifndef GPIO_H
#define GPIO_H

#include "stdint.h"

struct GPIO_REG
{
    volatile uint32_t FSEL0;    //0x00
    volatile uint32_t FSEL1;    //0x04
    volatile uint32_t FSEL2;    //0x08
    volatile uint32_t FSEL3;    //0x0c
    volatile uint32_t FSEL4;    //0x10
    volatile uint32_t FSEL5;    //0x14
    uint32_t          _res0;    //0x18
    volatile uint32_t  SET0;    //0x1c
    volatile uint32_t  SET1;    //0x20 
    uint32_t          _res1;    //0x24
    volatile uint32_t  CLR0;    //0x28
    uint32_t       _res2[2];    //0x2c, 0x30
    volatile uint32_t  LEV0;    //0x34
    volatile uint32_t  LEV1;    //0x38
    uint32_t          _res3;    //0x3c
    volatile uint32_t PEDS0;    //0x40
    volatile uint32_t PEDS1;    //0x44
    uint32_t       _res4[7];    //0x48, 0x4c, 0x50, 0x54, 0x58, 0x5c, 0x60
    volatile uint32_t  HEN0;    //0x64
    volatile uint32_t  HEN1;    //0x68
    uint32_t       _res5[9];    //0x6c, 0x70, 0x74, 0x78, 0x7c, 0x80, 0x84, 0x88, 0x8c, 0x90
    volatile uint32_t   PUD;    //0x94
    volatile uint32_t  PUDCLK0; //0x98
    volatile uint32_t  PUDCLK1; //0x9c
};

#define GPIO_BASE           0x3F200000
#define GP                  ((struct GPIO_REG*)GPIO_BASE)

#endif