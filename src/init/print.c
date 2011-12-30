#include <asm/io.h>
#include <kernel/system.h>
#include <kernel/console.h>
#include <kernel/printk.h>
#include <kernel/timer.h>
#include <kernel/isr.h>
#include <kernel/keyboard.h>
#include <kernel/exceptions.h>
#include <kernel/sched.h>
#include <mm/paging.h>

static void pic_install(void)
{
	outb(0x11, 0x20);
	outb(0x11, 0xa0);
	outb(0x20, 0x21);
	outb(0x28, 0xa1);
	outb(0x04, 0x21);
	outb(0x02, 0xa1);
	outb(0x01, 0x21);
	outb(0x01, 0xa1);
	outb(0xff, 0x21);
	outb(0xff, 0xa1);
}

void pause(void)
{
	asm("nop");
}

void main(void)
{
	char wheel[] = { '\\', '|', '/', '-' };
	char *videoram = (char *)0xb8000;
	int i;
	int count = 0;

	initialize_paging();
	console_init();
	pic_install();
	traps_init();
	timer_install(100);
	kb_install();
	sched_init();

	sti();

	for(;;)
	{
		if (i == sizeof (wheel))
			i = 0;

		asm("movb %%al, 0xb8000 + 160*24"::"a"(wheel[i++]));
	}

	return;
}
