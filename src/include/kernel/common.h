#ifndef COMMON_H
#define COMMON_H

typedef int size_t;

typedef char s8int;
typedef unsigned char u8int;
typedef short s16int;
typedef unsigned short u16int;
typedef int s32int;
typedef unsigned int u32int;
typedef long long s64int;
typedef unsigned long long u64int;

typedef int bool;
#define true 1
#define false 0

#define NULL ((void *)0)

#define BOCHS_DEBUG() asm volatile("xchg %bx, %bx")

#define ASSERT(x) ((x) ? 0 : printk(KPL_PANIC, "PANIC! %s:%s|||||| %s\n", __FILE__, __PRETTY_FUNCTION__, #x))

#endif /* COMMON_H */
