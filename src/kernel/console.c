#include <kernel/console.h>
#include <asm/io.h>
#include <kernel/mem.h>

/* cursor positions */
static int csr_x = 0;
static int csr_y = 0;

static void scroll_down(int lines)
{
	int i;
	char *p;
	short *src = (short *)(VIDEORAM_START + VRAM_LINE * lines);
	short *dst = (short *)(VIDEORAM_START);
	memcpy((void *)src, (void *)dst, (unsigned int)(VRAM_LINE * (LINES - lines)));

	i = (LINES - lines) * COLUMNS * 2;
	p = (char *)(VIDEORAM_START + VRAM_LINE * (LINES - lines));
	for (; i > 0; --i)
	{
		*p++ = EMPTY_CHAR;
		*p++ = BLANK_ATTR;
	}
}

void set_cursor(int x, int y)
{
	csr_x = x;
	csr_y = y;

	outb(0x0e, 0x3d4);
	outb(((csr_x + csr_y * COLUMNS) >> 8) & 0xff, 0x3d5);
	outb(0x0f, 0x3d4);
	outb(((csr_x + csr_y * COLUMNS)) & 0xff, 0x3d5);
}

void get_cursor(int *x, int *y)
{
	*x = csr_x;
	*y = csr_y;
}

void print_c(char c, COLOUR fg, COLOUR bg)
{
	char *dst = (char *)(VIDEORAM_START + csr_x*2 + csr_y*VRAM_LINE);
	char attr = (char)(bg << 4 | fg);

	switch (c)
	{
		case '\n':
			for (; csr_x < COLUMNS; ++csr_x)
			{
				*dst++ = EMPTY_CHAR;
				*dst++ = attr;
			}
			break;
		case '\r':
			csr_x = 0;
			break;
		case '\b':
			if ((!csr_x) && (!csr_y))
				return;
			if (!csr_x)
			{
				csr_x = COLUMNS - 1;
				--csr_y;
			} else
				--csr_x;
			dst[-1] = BLANK_ATTR;
			dst[-2] = EMPTY_CHAR;
			break;
		case '\t':
			c = csr_x + TAB_WIDTH;
			c = c < COLUMNS ? c : COLUMNS;
			for (; csr_x < c; ++csr_x)
			{
				*dst++ = EMPTY_CHAR;
				*dst++ = attr;
			}
			break;
		default:
			*dst++ = c;
			*dst++ = attr;
			++csr_x;
			break;
	}

	if (csr_x >= COLUMNS)
	{
		csr_x = 0;
		if (csr_y < (LINES - 1))
			csr_y++;
		else
			scroll_down(1);
	}
	set_cursor(csr_x, csr_y);
}
