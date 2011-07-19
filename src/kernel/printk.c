#include <kernel/console.h>
#include <kernel/printk.h>

#define va_list char * /* variable list of arguments */

#define va_rounded_size(TYPE) \
	(((sizeof(TYPE) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(AP, LASTARG) \
	(AP = ((char *)&(LASTARG) + va_rounded_size(LASTARG)))

#define va_end(AP)

#define va_next(AP, TYPE) (((TYPE *)(AP += va_rounded_size(TYPE)))[-1])

static char buffer[1024];
static int ptr = -1;

static void parse_hex(unsigned int num)
{
	int i = 8;
	while (i-- > 0)
	{
		buffer[ptr++] = "0123456789abcdef"[(num >> (i * 4)) & 0x0f];
	}
}

static void parse_num(unsigned int num, unsigned int base)
{
	unsigned int n = num / base;
	int r = num % base;
	if (r < 0)
	{
		r += base;
		--n;
	}

	if (num >= base)
		parse_num(n, base);
	buffer[ptr++] = "0123456789"[r];
}

void printk(enum KP_LEVEL kl, const char *fmt, ...)
{
	int i = 0;
	char *s;
	
	struct KPC_STRUCT {
		COLOUR fg;
		COLOUR bg;
	} KPL[] = {
		{BRIGHT_WHITE, BLACK},
		{YELLOW, RED},
	};

	va_list args;
	va_start(args, fmt);

	ptr = 0;

	for (; fmt[i]; ++i)
	{
		if ((fmt[i] != '%') && (fmt[i] != '\\'))
		{
			buffer[ptr++] = fmt[i];
			continue;
		} else if (fmt[i] == '\\')
		{
			switch(fmt[++i])
			{
				case 'a':
					buffer[ptr++] = '\a'; break;
				case 'b':
					buffer[ptr++] = '\b'; break;
				case 't':
					buffer[ptr++] = '\t'; break;
				case 'n':
					buffer[ptr++] = '\n'; break;
				case 'r':
					buffer[ptr++] = '\r'; break;
			}

			continue;
		}

		switch (fmt[++i])
		{
			case 's':
				s = (char *)va_next(args, char *);
				while (*s)
					buffer[ptr++] = *s++;
				break;
			case 'c':
				buffer[ptr++] = (char)va_next(args, char);
				break;
			case 'x':
				parse_hex((unsigned long)va_next(args, unsigned long));
				break;
			case 'd':
				parse_num((unsigned long)va_next(args, unsigned long), 10);
				break;
			case '%':
				buffer[ptr++] = '%';
				break;

			default:
				buffer[ptr++] = fmt[i];
				break;
		}
	}

	buffer[ptr] = '\0';
	va_end(args);
	for (i = 0; i < ptr; ++i)
		print_c(buffer[i], KPL[kl].fg, KPL[kl].bg);
}
