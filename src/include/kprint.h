#ifndef KPRINT_H
#define KPRINT_H

#include <console.h>

enum KP_LEVEL { KP_DUMP, KP_PANIC };

void printk(enum KP_LEVEL log_lvl, const char *fmt, ...);

#endif /* KPRINT_H */
