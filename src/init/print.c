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
#include <drivers/hd.h>
#include <drivers/pata.h>
#include <mm/memory.h>
#include <mm/kheap.h>

extern heap_t *kheap;

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

	u32int *test = (u32int *)0x200000;
	*test = 0xb119b00b;
	test = (u32int *)0x500000;
	*test = 0xdeadbeef;

	//printk(KPL_DUMP, "About to read from disk\n");
	//BOCHS_DEBUG();
	
	/*
	reset_hd_controller();
	initialize_pata();

	hd_request_t *req;
	buffer_head_t buffer;
	BOCHS_DEBUG();
	char *buf = (char *)kmalloc(512);
	memset(buf, 0x33, 512);
	BOCHS_DEBUG();
	buffer.block_data = buf;
	req = create_request(0, 2, 2, 1, 1, HD_REQUEST_TYPE_WRITE, &buffer);
	add_request_to_queue(&pending_requests, req);
	submit_request();
	//remove_request_from_queue(&pending_requests);
	*/

	int p;
	for (p = KHEAP_START; p < KHEAP_START + KHEAP_INITIAL_SIZE; p += 0x1000)
		put_page((u32int)get_free_page(), p);
	
	kheap = kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000);
	BOCHS_DEBUG();
	u32int *a = kmalloc(8);
	BOCHS_DEBUG();
	u32int *b = kmalloc(8);
	BOCHS_DEBUG();
	kfree(a);
	kfree(b);
	BOCHS_DEBUG();
	u32int *c = kmalloc(0x100000);
	BOCHS_DEBUG();

	//outb(inb(0x21) & 0xfb, 0x21);
	//outb(inb(0xa1) & 0xbf, 0xa1);
	//outb(0xa0, ATA_PRI_HEAD);
	//reset_hd_controller();
	//printk(KPL_DUMP, "Reset the controller, now reading\n");
	//hd_rw(0, 2, 6, 2, 1, 0x20);

	//BOCHS_DEBUG();

	//calc_mem();

	for(;;)
	{
		if (i == sizeof (wheel))
			i = 0;

		asm("movb %%al, 0xb8000 + 160*24"::"a"(wheel[i++]));
	}

	return;
}
