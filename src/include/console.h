#ifndef CONSOLE_H
#define CONSOLE_H

#define MAX_LINES	25
#define MAX_COLUMNS	80

#define VRAM_LINE	(MAX_COLUMNS * 2)

#define VRAM		0xB8000

#define VRAM_OFFSET(x,y)	(VRAM_LINE*(y)+2*(x))

#define TAB_WIDTH	8	/* a tab character is going to be 4 chars long (8 bytes) */

typedef enum COLOUR_ATTR
{
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN,
	LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN,
	LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE
} COLOUR;

void set_cursor(int x, int y);
void get_cursor(int *x, int *y);
void print_char(char c, COLOUR fg, COLOUR bkg);

#endif /* CONSOLE_H */
