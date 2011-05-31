#ifndef _PRINTK__INCLUDED_
#define _PRINTK__INCLUDED_

#include "kernel/scr.h"

enum KP_LEVEL { KPL_DUMP, KPL_PANIC };

void printk(enum KP_LEVEL lvl, const char *fmt, ...);

#endif /* _PRINTK__INCLUDED_*/
