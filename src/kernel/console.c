#include <kernel/common.h>
#include <kernel/console.h>
#include <asm/io.h>
#include <kernel/mem.h>
#include <kernel/system.h>

/* cursor position */
static int csr_x, csr_y; /* in character, line units (canonical units) */
static u32int position = VIDEORAM_START; /* in memory units */
static u32int origin = VIDEORAM_START; /* The memory address the CRT controller should use while refreshing */
static u32int scr_end = VIDEORAM_START + COLUMNS * LINES * 2; /* As above but it's the bottom */
static u32int top = 0, bottom = LINES; /* top and bottom of screen in canonical units */

static void scroll_up(void)
{
	if (!top && bottom == LINES)
	{
		position += COLUMNS * 2;
		origin += COLUMNS * 2;
		scr_end += COLUMNS * 2;

		if (scr_end > VIDEORAM_END)
		{
			asm("cld\n\t"
					"rep\n\t"
					"movsl\n\t"
					"movl %%ebx, %1\n\t"
					"rep\n\t"
					"stosw"
					:: "a"(0x0720),
					"c"((LINES - 1) * COLUMNS >> 1),
					"D"(VIDEORAM_START),
					"S"(origin),
					"b"(COLUMNS));

			scr_end -= origin - VIDEORAM_START;
			position -= origin - VIDEORAM_START;
			origin = VIDEORAM_START;
		}
		else
		{
			asm("cld\n\t"
					"rep\n\t"
					"stosl"
					:: "a"(0x07200720),
					"c"(COLUMNS >> 1),
					"D"(scr_end - (COLUMNS << 1)));
		}

		set_origin();
	} else
	{
		asm("cld\n\t"
				"rep\n\t"
				"movsl\n\t"
				"movl %%ebx, %%ecx\n\t"
				"rep\n\t"
				"stosw"
				:: "a"(0x0720),
				"c"((bottom - top - 1) * COLUMNS >> 1),
				"D"(origin + (COLUMNS << 1) * top),
				"S"(origin + (COLUMNS << 1) * (top + 1)),
				"b"(COLUMNS));
	}
}

/*
static void scroll_down(void)
{
	asm("std\n\t"
			"rep\n\t"
			"movsl\n\t"
			"addl $2, %%edi\n\t"
			"movl %%ebx, %%ecx\n\t"
			"rep\n\t"
			"stosw"
			:: "a"(0x0720),
			"b"(COLUMNS),
			"c"((bottom - top - 1) * COLUMNS >> 1),
			"D"(origin + (COLUMNS << 1) * bottom - 4),
			"S"(origin + (COLUMNS << 1) * (bottom - 1) - 4));
}
*/

static void scroll_down(void)
{
	int i;
	unsigned short blank;
	unsigned short *vga_mem = (unsigned short *)VIDEORAM_START;

	blank = 0x20 | (((0 << 4) | (15 & 0xff)) << 8);

	memcpy(vga_mem + COLUMNS, vga_mem, COLUMNS * (LINES - 1) * 2);

	for (i = 0; i < COLUMNS; ++i)
	{
		vga_mem[COLUMNS * (LINES - 1) + i] = blank;
	}
}


void clear_screen(void)
{
	unsigned short blank;
	unsigned short *vga_mem = (unsigned short *)VIDEORAM_START;

	blank = 0x20 | (((0 << 4) | (15 & 0xff)) << 8);

	memsetw(vga_mem, blank, LINES * COLUMNS);
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

void set_origin(void)
{
	cli();

	/* Send first byte divided by 2. The address is relative to the start */
	outb(0x0c, 0x3d4);
	outb(0xff & ((origin - VIDEORAM_START) >> 9), 0x3d5);

	/* Second byte / 2 */
	outb(0x0d, 0x3d4);
	outb(0xff & ((origin - VIDEORAM_START) >> 1), 0x3d5);

	sti();
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

void console_init(void)
{
	int i;
	clear_screen();
	set_cursor(0, 0);

	for (i = 0; i < 10; ++i)
		scroll_down();
}
