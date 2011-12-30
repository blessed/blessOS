#include <kernel/mem.h>

void *memcpy(const void *src, void *dst, size_t n)
{
	const char *sp = (const char *)src;
	char *dp = (char *)dst;

	for (; n != 0; --n)
		*dp++ = *sp++;

	return dst;
}

void *memset(void *dest, char val, size_t count)
{
	char *temp = (char *)dest;
	for (; count != 0; --count)
		*temp++ = val;
	return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	unsigned short *temp = dest;
	for (; count != 0; --count)
		*temp++ = val;
	return dest;
}
