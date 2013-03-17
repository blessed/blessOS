/* keyboard.c - contains the keyboard handling code */

#include <kernel/printk.h>
#include <kernel/console.h>
#include <kernel/keyboard.h>
#include <asm/io.h>
#include <kernel/cyclic_buffer.h>
#include <kernel/tty.h>

extern tty_t main_tty;

/* Muahahahaha! Try to guess all the keys... The me from the past was a mean bastard :> */
static keytype_t keytable[255] = {
	KT_NONE, [1 ... 28] = KT_NORMAL, KT_LCTRL,
	[0x1e ... 0x29] = KT_NORMAL, KT_LSHIFT, [0x2b ... 0x35] = KT_NORMAL,
	KT_RSHIFT, KT_NORMAL, 
	KT_ALT, KT_NORMAL, KT_CAPSLOCK,
	[0x3b ... 0x44] = KT_FUNC, KT_NUMLOCK, KT_SCROLL,
	[0x47 ... 0x48] = KT_CURSOR, KT_NORMAL, [0x4b ... 0x4d] = KT_CURSOR,
	KT_NORMAL, [0x4f ... 0x53] = KT_CURSOR,
	KT_NONE, KT_NONE, KT_NORMAL,
	KT_FUNC, KT_FUNC, [0x59 ... 0xfe] = KT_NONE
};

static keymodifier_t key_mode;

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

u8int shift_kbdus[128] = {
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
	'~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*', 0,
	' ',
	0,
	0,
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
		scancode &= ~0x80;

		switch (keytable[scancode])
		{
			case KT_LSHIFT:
				key_mode &= ~KM_LSHIFT;
				break;

			case KT_LCTRL:
				key_mode &= ~KM_CTRL;
				break;

			case KT_ALT:
				key_mode &= ~KM_ALT;
				break;

			default:
				break;
		}
	}
	else
	{
		switch (keytable[scancode])
		{
			case KT_NORMAL:
				if (key_mode & KM_LSHIFT)
					scancode = shift_kbdus[scancode];
				else
					scancode = kbdus[scancode];

				cb_push(scancode, &main_tty.read_queue);
				do_tty_interrupt(&main_tty);
				break;

			case KT_RSHIFT:
			case KT_LSHIFT:
				key_mode |= KM_LSHIFT;
				break;

			case KT_LCTRL:
				key_mode |= KM_CTRL;
				break;

			case KT_ALT:
				key_mode |= KM_ALT;
				break;

			default:
				break;
		}
	}
	
	outb(0xA0, 0x20);
	outb(0x20, 0x20);
}

void
kb_install(void)
{
	outb(inb(0x21)&0xfd, 0x21);
	cb_init(&keyboard_buf);
}
