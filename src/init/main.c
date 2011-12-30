#include <asm/io.h>
#include <kernel/system.h>
#include <kernel/printk.h>
#include <kernel/timer.h>
#include <kernel/isr.h>
#include <kernel/keyboard.h>
#include <kernel/exceptions.h>

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

int main()
{
	int count = 0;
	int i = 0;
	int a = 3, b = 0;
	char hello[] = "Hello to my OS!";
	unsigned char *videoram = (unsigned char *) 0xB8000; /* 0xB0000 for monochrome monitors */

	unsigned int *task_reg = 0x200000;

	pic_install();
	traps_init();
	timer_install(100);
	kb_install();

	/* clear screen */
	for(i=0; i < 16000; ++i)
	{
		videoram[count++] = 'c';
		videoram[count++] = 0x00;  /* print black 'A' on black background */
	}

	asm volatile (
			"str %0"
			: "=m"(task_reg)
			:
			);

	printk(KPL_DUMP, "task reg %x\n", *task_reg);
 
	/* print string */
	i = 0;
	count = 0;
	while(hello[i] != '\0')
	{
		videoram[count++] = hello[i++];
		videoram[count++] = 0x07; /* grey letters on black background */
	}

	while(1); /* just spin */
 
	return 0;
}
