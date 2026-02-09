#ifndef PRINTF_H
#define PRINTF_H

#include "lareos.h"

void kprintf(const char *fmt, ...);
int ksprintf(char *buf, const char *fmt, ...);

typedef __builtin_va_list va_list;
#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_arg(v,l)   __builtin_va_arg(v,l)

#endif
