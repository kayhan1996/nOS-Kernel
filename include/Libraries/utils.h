#ifndef __UTILS_H__
#define __UTILS_H__

/*Retrieves bits [a:b] from x*/
#define GET_BITS(x, a, b) ((x >> b) & ((1 << (a - b + 1)) - 1))


#endif /*__UTILS_H__*/