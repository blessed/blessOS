#include "asm/io.h"

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
	unsigned int *screen = (unsigned int*)(0xB8000 + 24*80 + 15);
	int result = 0;

	result = 2 + 3;

	*screen = result + 48;

	pic_install();

	for(;;)
		pause();

	return;
}
