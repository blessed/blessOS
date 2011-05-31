#include <asm/io.h>
#include <kernel/libcc.h>
#include <console.h>

static int cur_x = 0;
static int cur_y = 0;

void set_cursor(int x, int y)
{
	cur_x = x;
	cur_y = y;
	outb(0x0e, 0x3d4);
	outb((((cur_x + cur_y * MAX_COLUMNS) >> 8) && 0xff), 0x3d5);
	outb(0x0f, 0x3d4);
	outb((((cur_x + cur_y * MAX_COLUMNS)) && 0xff), 0x3d5);
}

void get_cursor(int *x, int *y)
{
	*x = cur_x;
	*y = cur_y;
}

static void scroll_down(unsigned int lines)
{
	unsigned short *p = (unsigned short *)(VRAM + VRAM_LINE * (MAX_LINES - lines));
	int i = MAX_COLUMNS * lines - 1;
	/* copy the contents of the framebuffer x lines up */
	memcpy((void *)(VRAM), (void *)(VRAM + VRAM_LINE * lines), VRAM_LINE * (MAX_LINES - lines));
	for (; i > 0; --i)
	{
		*p++ = 0x0720;
	}
}

void print_char(char c, COLOUR fg, COLOUR bkg)
{
	unsigned char *p = (unsigned char *)(VRAM + VRAM_OFFSET(cur_x, cur_y));
	unsigned char attribute = ((fg << 4) | bkg);

	switch (c)
	{
		case '\r':
			cur_x = 0;
			break;
		case '\n':
			for (; cur_x < MAX_COLUMNS; ++cur_x)
			{
				*p++ = 0x20; /* space character */
				*p++ = attribute;
			}
			break;
		case '\t':
			c = cur_x + TAB_WIDTH - (cur_x & (TAB_WIDTH - 1)); /* check if the tab fits the screen */
			c = c < MAX_COLUMNS ? c : MAX_COLUMNS;
			for (; cur_x < c; ++cur_x)
			{
				*p++ = 0x20;
				*p++ = attribute;
			}
			break;
		case '\b':
			if (!cur_x && !cur_y)
				return;
			if (!cur_x)
			{
				cur_x = MAX_COLUMNS - 1;
				cur_y--;
			} else
				cur_x--;
			((short *)p)[-1] = 0x0720;
			break;
			
		default:
			*p++ = c;
			*p++ = attribute;
			cur_x++;
			break;
	}

	if (cur_x >= MAX_COLUMNS)
	{
		cur_x = 0;
		if (cur_y < MAX_LINES-1)
			cur_y++;
		else
			scroll_down(1);
	}
	set_cursor(cur_x, cur_y);
}
