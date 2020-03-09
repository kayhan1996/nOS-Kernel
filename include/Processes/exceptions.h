#include "stdint.h"

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

void error_handler(uint64_t FAR, uint64_t ESR, uint64_t ELR);

#endif /* __EXCEPTIONS_H__ */