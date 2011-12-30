#include <kernel/timer.h>
#include <kernel/console.h>
#include <kernel/printk.h>
#include <asm/io.h>
#include <kernel/sched.h>

extern struct task_struct task0;

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
	struct task_struct *v = &task0;

	++timer_ticks;
	get_cursor(&x, &y);
	set_cursor(71, 24);
	printk(KPL_DUMP, "%x", timer_ticks);
	set_cursor(x, y);
	outb(0x20, 0x20);
	cli();
	for (; v; v=v->next)
	{
		if (v->state == TS_RUNNING)
		{
			if ((v->priority += 30) <= 0)
				v->priority = 0xff;
		}
		else
			v->priority -= 10;
	}
	schedule();
	sti();
}

