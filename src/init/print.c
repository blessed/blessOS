#include <asm/io.h>
#include <kernel/system.h>
#include <kernel/console.h>
#include <kernel/config.h>
#include <kernel/printk.h>
#include <kernel/timer.h>
#include <kernel/isr.h>
#include <kernel/keyboard.h>
#include <kernel/exceptions.h>
#include <kernel/sched.h>
#include <mm/memory.h>

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
	int i;

	//initialise_paging();
	console_init();
	pic_install();
	traps_init();
	timer_install(HZ);
	kb_install();
	
	printk(KPL_DUMP, "The gdt is at 0x%x\n", gdt);
	u32int *page = get_free_page();
	printk(KPL_DUMP, "Got free page at %d\n", (u32int)page);
	put_page((u32int)page, 0x200000);
	
	sched_init();

	sti();

	u32int *test = 0x200000;
	*test = 0xb119b00b;
	test = 0x500000;
	*test = 0xdeadbeef;

	BOCHS_DEBUG();

	calc_mem();

	for(;;)
	{
		if (i == sizeof (wheel))
			i = 0;

		asm("movb %%al, 0xb8000 + 160*24"::"a"(wheel[i++]));
	}

	return;
}
