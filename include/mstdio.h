#include "stdint.h"

#ifndef MSTDIO_H
#define MSTDIO_H
void println(const char *str);
void print(const char *str);
void printhex(uint64_t number);
void printdec(unsigned int number);

uint64_t gethex();
#endif