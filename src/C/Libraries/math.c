#include <Libraries/math.h>


/* Divides the numerator n with denominator d
   and calculates the ceiling of the resulting division.
   REQUIRES: 32-bit unsigned integers n and d           */
uint32_t ceiling(uint32_t n, uint32_t d) {
    uint32_t p = n / d;
    uint32_t r = n - (p * d);
    if (r > 0) {
        return (p + 1);
    } else {
        return p;
    }
}

/* Rounds an unsigned integer n to the nearest power of 2. */
/* REQUIRES: 32-bit unsigned integer n                     */
uint32_t round_to_power2(uint32_t n) {
    for (int i = 31; i >= 0; i--) {
        uint32_t power = 1 << i;
        if (power == n) {
            return power;
        } else if (power < n) {
            return power << 1;
        }
    }

    return 0;
}