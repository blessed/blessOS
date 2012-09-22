#include <kernel/console.h>
#include <kernel/printk.h>
#include <kernel/tty.h>
#include <kernel/cyclic_buffer.h>

#define _L_FLAG(tty,f)	((tty)->termios.c_lflag & f)
#define _I_FLAG(tty,f)	((tty)->termios.c_iflag & f)
#define _O_FLAG(tty,f)	((tty)->termios.c_oflag & f)

#define L_CANON(tty)	_L_FLAG((tty),ICANON)
#define L_ISIG(tty)		_L_FLAG((tty),ISIG)
#define L_ECHO(tty)		_L_FLAG((tty),ECHO)
#define L_ECHOE(tty)	_L_FLAG((tty),ECHOE)
#define L_ECHOK(tty)	_L_FLAG((tty),ECHOK)
#define L_ECHOCTL(tty)	_L_FLAG((tty),ECHOCTL)
#define L_ECHOKE(tty)	_L_FLAG((tty),ECHOKE)

#define I_UCLC(tty)		_I_FLAG((tty),IUCLC)
#define I_NLCR(tty)		_I_FLAG((tty),INLCR)
#define I_CRNL(tty)		_I_FLAG((tty),ICRNL)
#define I_NOCR(tty)		_I_FLAG((tty),IGNCR)

#define O_POST(tty)		_O_FLAG((tty),OPOST)
#define O_NLCR(tty)		_O_FLAG((tty),ONLCR)
#define O_CRNL(tty)		_O_FLAG((tty),OCRNL)
#define O_NLRET(tty)	_O_FLAG((tty),ONLRET)
#define O_LCUC(tty)		_O_FLAG((tty),OLCUC)

extern circular_buf_t keyboard_buf;

tty_t main_tty = {
	tty_write,
	{ "", 0, 0 },
	{ "", 0, 0 },
	{ 0, OPOST | ONLCR, 0, ICANON | ECHO | ECHOCTL | ECHOKE, INIT_C_CC }
};

void tty_write(tty_t *tty)
{
	//printk(KPL_DUMP, "%s:%d\n", __FILE__, __LINE__);
	u32int char_count = cb_get_count(&tty->write_queue);

	while (char_count--)
		print_c(cb_pop(&tty->write_queue), BRIGHT_WHITE, BLACK);
}

void parse_key(tty_t *tty)
{
	//printk(KPL_DUMP, "Properly got to parse_key\n");
	//print_c(cb_pop(&tty->read_queue), BRIGHT_WHITE, BLACK);
	u8int c;

	while (!cb_is_empty(&tty->read_queue))
	{
		c = cb_pop(&tty->read_queue);

		if (c == KBD_CR)
		{
			if (I_CRNL(tty))
				c = KBD_LF;
			else if(I_NOCR(tty))
				continue;
		} else if (c == KBD_LF && I_NLCR(tty))
			c = KBD_CR;

		if (L_ECHO(tty))
		{
			if (c == KBD_LF)
			{
				cb_push(c, &tty->write_queue);
				c = KBD_CR;
				cb_push(c, &tty->write_queue);
			} else if (c >= 32)
				cb_push(c, &tty->write_queue);

			tty->write(tty);
		}
	}
}

void do_tty_interrupt(tty_t *tty)
{
	parse_key(tty);
}
