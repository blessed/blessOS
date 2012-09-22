#include <kernel/config.h>
#include <kernel/printk.h>
#include <kernel/sched.h>
#include <mm/memory.h>
#include <kernel/isr.h>

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

	mem_map[i] = 1;

	/* return the physical addr of page based on the mem_map vector index */
	return (u32int *)((i << 12) + LOW_MEM);
}

u32int put_page(u32int page, u32int address)
{
	u32int tmp;
	u32int *page_table;

	if (page < LOW_MEM || page > HIGH_MEM)
		printk(KPL_PANIC, "Trying to put page %p at %p\n", page, address);

	if (mem_map[(page - LOW_MEM) >> 12] != 1)
		printk(KPL_PANIC, "mem_map disagrees with %p at %p\n", page, address);

	/* Check if the page dir entry for address is present
	 * if yes, then use it's information for the base of the 2nd level page table.
	 * If not, create the proper page dir entry and use it */
	/* NOTE: shifting right only by 20 bits so that the calculated address is in 4byte units */
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

void free_page(u32int address)
{
	printk(KPL_DUMP, "%s: address = %d\n", __FUNCTION__, address);

	if (address < LOW_MEM)
		return;

	if (address > HIGH_MEM)
		printk(KPL_PANIC, "Trying to free page behind available memory\n");

	address -= LOW_MEM;
	address >>= 12;

	if (mem_map[address]--)
		return;

	mem_map[address] = 0;

	printk(KPL_PANIC, "Trying to free free page\n");
}

void no_page(u32int error, u32int address)
{
	u32int tmp = (u32int)get_free_page();

	if (tmp)
		if (put_page(tmp, address))
			return;

	printk(KPL_PANIC, "Couldn't allocate new page!!!\n");
}

void calc_mem(void)
{
	u32int *pg_tbl;
	int i, j, k;
	u32int free = 0;

	for (i = 0; i < PAGING_PAGES; ++i)
		if (!mem_map[i]) free++;

	printk(KPL_DUMP, "%d pages free of %d\n", free, PAGING_PAGES);
	printk(KPL_DUMP, "There are %d KiB free\n", free * 4096 / 1024);

	for (i = 2; i < 1024; ++i)
	{
		if (1 & pg_dir[i])
		{
			pg_tbl = (u32int *)(0xfffff000 & pg_dir[i]);

			for (j = k = 0; j < 1024; ++j)
			{
				if (pg_tbl[j] & 1)
					k++;
			}

			printk(KPL_DUMP, "Pg-dir[%d] uses %d pages\n", i, k);
		}
	}
}
