#include <kernel/console.h>
#include <asm/io.h>
#include <kernel/mem.h>

/* cursor position */
static int csr_x, csr_y;

/* something is fucked up with the scroll_down procedure.
 * gotta look into this in detail
 */

static void scroll_down()
{
	int i;
	unsigned short *vga_mem = (unsigned short *)VIDEORAM_START;

	for (i = 0; i < COLUMNS * (LINES-1); ++i)
		vga_mem[i] = vga_mem[i + COLUMNS];

	for (i = 0; i < COLUMNS; ++i)
	{
		vga_mem[COLUMNS * (LINES - 1) + i] = 3872;
	}
}

void set_cursor(int x, int y)
{
	unsigned short position;
	csr_x = x;
	csr_y = y;

	position = csr_y * COLUMNS + csr_x;

	outb(0x0f, 0x3d4);
	outb((unsigned char)(position & 0xff), 0x3d5);
	outb(0x0e, 0x3d4);
	outb((unsigned char)((position >> 8) & 0xff), 0x3d5);
}

void get_cursor(int *x, int *y)
{
	*x = csr_x;
	*y = csr_y;
}

void print_c(char c, COLOUR fg, COLOUR bg)
{
	unsigned char *dest = (unsigned char *)(VIDEORAM_START + csr_y * VRAM_LINE + csr_x * 2);
	unsigned char attr = (unsigned char)(bg << 4 | fg);

	switch (c)
	{
		case '\r':
			csr_x = 0;
			break;
		case '\n':
			for (; csr_x < COLUMNS; ++csr_x)
			{
				*dest++ = (unsigned char)EMPTY_CHAR;
				*dest++ = attr;
			}
			break;
		case '\t':
			c = csr_x + TAB_WIDTH - (csr_x & (TAB_WIDTH - 1));
			c = c < COLUMNS ? c : COLUMNS;
			for (; csr_x < c; ++csr_x)
			{
				*dest++ = (unsigned char)EMPTY_CHAR;
				*dest++ = attr;
			}
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
			dest[-1] = (unsigned char)BLANK_ATTR;
			dest[-2] = (unsigned char)EMPTY_CHAR;
			break;
		default:
			*dest++ = c;
			*dest++ = attr;
			++csr_x;
			break;
	}

	if (csr_x >= COLUMNS)
	{
		csr_x = 0;
		if (csr_y < (LINES - 1))
			++csr_y;
		else
			scroll_down();
	}
	set_cursor(csr_x, csr_y);
}
