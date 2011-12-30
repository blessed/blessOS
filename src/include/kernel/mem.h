#ifndef _MEM_H_
#define _MEM_H_

#include <kernel/common.h>

void *memcpy(const void *src, void *dst, size_t count);
void *memset(void *dest, char val, size_t count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);

#endif /* _MEM_H_ */
