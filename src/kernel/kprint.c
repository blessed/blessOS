#include <kprint.h>

typedef char * va_list;

#define va_rounded_size(type) \
	(((sizeof(type) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, last_arg) \
	(ap = (char *)&last_arg + va_rounded_size(last_arg))

#define va_next(ap, type) \
	(((type *)(ap += va_rounded_size(type)))[-1])

#define va_end(ap)

static unsigned char buffer[1024] = { 1 };
static int ptr = -1;

static void parse_hex(unsigned long num)
{
	int i = 8;
	while (i-- > 0)
		buffer[ptr++] = "0123456789abcdef"[(num >> (i * 4)) & 0xf];
}

static void parse_num(unsigned long num, unsigned int base)
{
	int n = num / base;
	int r = num % base;
	
	if (num >= base)
		parse_num(n, base);
	buffer[ptr++] = "0123456789"[r];
}

void printk(enum KP_LEVEL log_lvl, const char *fmt, ...)
{
	int i = 0;
	char *s;

	struct KPC_STRUCT
	{
		COLOUR fg;
		COLOUR bkg;
	} kpc[] = 
	{
		{ WHITE, BLACK },
		{ YELLOW, RED }
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
			switch (fmt[++i])
			{
				case '\a':
					buffer[ptr++] = '\a';
					break;
				case '\b':
					buffer[ptr++] = '\b';
					break;
				case '\r':
					buffer[ptr++] = '\r';
					break;
				case '\t':
					buffer[ptr++] = '\t';
					break;
				case '\n':
					buffer[ptr++] = '\n';
					break;
				case '\\':
					buffer[ptr++] = '\\';
					break;
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
				buffer[ptr++] = (char)va_next(args, int);
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
	{
		print_char(buffer[i], kpc[log_lvl].fg, kpc[log_lvl].bkg);
	}
}
