#include <kernel/mem.h>

void memcpy(const void *src, void *dst, unsigned int n)
{
	const char *s = src;
	char *d = dst;
	/* check if dst overlaps src */
	if (s <= d)
		for (; n > 0; --n)
		{
			d[n-1] = s[n-1];
		}
	else
		for (; n > 0; --n)
			*d++ = *s++;
}
