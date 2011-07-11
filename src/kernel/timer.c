#include <kernel/timer.h>
#include <kernel/console.h>
#include <kernel/printk.h>
#include <asm/io.h>

void timer_install(int hz)
{
	unsigned int divisor = 1193180 / hz;
	outb(0x36, 0x43);
	outb(divisor & 0xff, 0x40);
	outb(divisor >> 8, 0x40);
	outb(inb(0x21) & ~0x01, 0x21);
}

volatile unsigned int timer_ticks = 0;

void do_timer(void)
{
	int x, y;
	++timer_ticks;
	get_cursor(&x, &y);
	set_cursor(71, 24);
	printk(KPL_DUMP, "%x", timer_ticks);
	set_cursor(x, y);
	outb(0x20, 0x20);
}

