#ifndef __MATH_H__
#define __MATH_H__

#include <stdint.h>

/* Divides the integer numerator n with denominator d
   and calculates the ceiling of the resulting division.
   REQUIRES: 32-bit unsigned integers n and d           */
uint32_t ceiling(uint32_t n, uint32_t d);

/* Rounds an unsigned integer n to the nearest power of 2
   REQUIRES: 32-bit unsigned integer n                    */
uint32_t round_to_power2(uint32_t n);

#endif /* __MATH_H__ */