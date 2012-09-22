#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#define VIDEORAM_START	0xB8000
#define VIDEORAM_END	0xC0000 /* VRAM is 32KiB long */

#define LINES			25
#define COLUMNS 		80
#define NPAR			16 /* maximum number of parameters */

#define VRAM_LINE		(COLUMNS * 2)

#define TAB_WIDTH		4
#define EMPTY_CHAR		0x20
#define	BLANK_ATTR		0x07

typedef enum COLOUR_TAG {
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE,
	GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, 
	LIGHT_RED, LIGHT_MAGENTA, YELLOW, BRIGHT_WHITE
} COLOUR;

void print_c(char c, COLOUR fg, COLOUR bg);

void set_cursor(int x, int y);
void get_cursor(int *x, int *y);
void set_origin(void);

void console_init(void);

#endif /* _CONSOLE_H_ */
