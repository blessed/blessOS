#include <kernel/config.h>
#include <kernel/printk.h>
#include <kernel/sched.h>
#include <mm/memory.h>

static u16int mem_map[PAGING_PAGES];

/* Returns physical address of found free page */
u32int* get_free_page(void)
{
	int i = PAGING_PAGES - 1;

	while (i > 0)
	{
		if (mem_map[i] == 0)
			break;
		--i;
	}

	if (i < 0)
		return NULL;

	/* return the physical addr of page based on the mem_map vector index */
	return (u32int *)((i << 12) + LOW_MEM);
}

u32int put_page(u32int page, u32int address)
{
	u32int tmp;
	u32int *page_table;

	if (page < LOW_MEM || page > HIGH_MEM)
		printk(KPL_PANIC, "Trying to put page %p at %p\n", page, address);

	if (mem_map[page - LOW_MEM] != 1)
		printk(KPL_PANIC, "mem_map disagrees with %p at %p\n", page, address);

	/* Check if the page dir entry for address is present
	 * if yes, then use it's information for the base of the 2nd level page table.
	 * If not, create the proper page dir entry and use it */
	u32int *page_dir_entry = (u32int *)((address >> 20) & 0xffc);

	if (*page_dir_entry & 1)
		page_table = (u32int *)(0xfffff000 & *page_dir_entry);
	else
	{
		tmp = (u32int)get_free_page();
		if (!tmp)
			return 0;

		/* enter the found page into page dir */
		*page_dir_entry = tmp | 7;
		page_table = (u32int *)tmp;
	}

	page_table[(address >> 12) & 0x3ff] = page | 7;

	printk(KPL_DUMP, "Put page was successful\n");
	
	return page;
}

void no_page(u32int error, u32int address)
{
	u32int tmp = (u32int)get_free_page();

	if (tmp)
		if (put_page(tmp, address))
			return;

	printk(KPL_PANIC, "Couldn't allocate new page!!!\n");
}

void do_page_fault(void)
{
	printk(KPL_DUMP, "Dupa\n");
}
