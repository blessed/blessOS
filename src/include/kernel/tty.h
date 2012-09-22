#ifndef TTY_H
#define TTY_H

#include <kernel/cyclic_buffer.h>
#include <termios.h>

#define KBD_CR			0x0d
#define KBD_LF			0x0a
#define KBD_BCKSPC		0x08
#define KBD_ESC			0x1b
#define KBD_DEL			0x7f

typedef struct tty
{
	void (*write)(struct tty *tty);
	circular_buf_t read_queue;
	circular_buf_t write_queue;
	termios_t termios;
} tty_t;

#define INIT_C_CC "\003\034\177\025\004\0\1\0\021\023\031\0\022\017\027\026\0"

void tty_write(tty_t *tty);

void do_tty_interrupt(tty_t *tty);
void parse_key(tty_t *tty);

#endif /* TTY_H */
