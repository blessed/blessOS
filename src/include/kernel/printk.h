#ifndef PRINTK_H
#define PRINTK_H

enum KP_LEVEL { KPL_DUMP, KPL_PANIC };

void printk(enum KP_LEVEL kl, const char *fmt, ...);

#endif /* PRINTK_H */
