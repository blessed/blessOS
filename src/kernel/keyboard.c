/* keyboard.c - contains the keyboard handling code */

#include <kernel/printk.h>
#include <kernel/console.h>
#include <kernel/keyboard.h>
#include <asm/io.h>
#include <kernel/cyclic_buffer.h>

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,					/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

circular_buf_t keyboard_buf;

void
do_kb(void)
{
	unsigned char scancode;

	scancode = inb(0x60);

	if (scancode & 0x80)
	{
	}
	else
	{
		print_c(kbdus[scancode], BRIGHT_WHITE, BLACK);
	}

	if (scancode == 28)
		printk(KPL_DUMP, "%s:%d: there are %d elements in buffer\n", __FILE__, __LINE__, cb_get_count(&keyboard_buf));
	
	outb(0xA0, 0x20);
	outb(0x20, 0x20);
}

void
kb_install(void)
{
	outb(inb(0x21)&0xfd, 0x21);
	BOCHS_DEBUG();
	cb_init(&keyboard_buf);
}
