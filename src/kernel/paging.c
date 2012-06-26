#include <mm/kheap.h>
#include <kernel/mem.h>
#include <mm/paging.h>
#include <kernel/printk.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory = NULL;

u32int *frames = NULL;
u32int nframes;

extern u32int placement_address;

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

static void set_frame(u32int addr)
{
	u32int frame = addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

static void clear_frame(u32int addr)
{
	u32int frame = addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

static u32int find_free_frame(void)
{
	u32int i, j;

	for (i = 0; i < INDEX_FROM_BIT(nframes); ++i)
	{
		if (frames[i] != 0xFFFFFFFF)
		{
			for (j = 0; j < 32; ++j)
			{
				u32int toTest = 0x1 << j;
				if (!(frames[i] & toTest))
				{
					return i * 4 * 8 + j;
				}
			}
		}
	}

	return (u32int)-1;
}

void alloc_new_page(page_t *page, u8int is_kernel, u8int is_writeable)
{
	if (page == NULL)
		return;

	if (page->frame != 0)
	{
		printk(KPL_DUMP, "Page already allocated\n");
		return;
	}
	else
	{
		u32int idx = find_free_frame();
		if (idx == (u32int)-1)
		{
			printk(KPL_PANIC, "Couldn't find free frames!\n");
		}
		
		set_frame(idx * 0x1000);
		page->present = 1;
		page->rw = is_writeable ? 1 : 0;
		page->user = is_kernel ? 0 : 1;
		page->frame = idx;
	}
}

void free_page(page_t *page)
{
	u32int frame;
	
	if (page == NULL)
		return;

	if (!(frame = page->frame))
	{
		return;
	}
	else
	{
		clear_frame(frame);
		page->frame = 0x0;
	}
}

page_t* get_page(u32int address, u8int make, page_directory_t *dir)
{
	address /= 0x1000;
	u32int idx = address / 1024;

	if (dir == NULL)
	{
		printk(KPL_DUMP, "Failed to get page due to lack of dir\n");
		return NULL;
	}

	if (dir->tables[idx])
	{
		return &dir->tables[idx]->pages[address % 1024];
	}
	else if (make)
	{
		u32int tmp;
		printk(KPL_DUMP, "Allocating page for 0x%x\n", address);
		dir->tables[idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
		dir->tablesPhysical[idx] = tmp | 0x07; /* present, rw, kernel */
		return &dir->tables[idx]->pages[address % 1024];
	}
	else
	{
		printk(KPL_DUMP, "Failed to get page\n");
		return NULL;
	}
}

void switch_page_directory(page_directory_t *new_dir)
{
	if (new_dir == NULL)
	{
		printk(KPL_DUMP, "new_dir is NULL\n");
		return;
	}

	printk(KPL_DUMP, "Switching page_directory_t to 0x%x\n", new_dir);
	printk(KPL_DUMP, "page tables are at 0x%x\n", new_dir->tablesPhysical);
	printk(KPL_DUMP, "firts page is at 0x%x\n", new_dir->tablesPhysical[0]);

	current_directory = new_dir;
	asm volatile ("mov %0, %%cr3" : : "r"(new_dir->tablesPhysical));
	u32int cr0;
	asm volatile ("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r"(cr0));
}

void initialise_paging(void)
{
	u32int mem_end_page = 0x800000;

	nframes = mem_end_page / 0x1000;
	printk(KPL_DUMP, "Initializing paging up to %d MiB, number of frames 0x%x\n", mem_end_page / 1024 / 1024, nframes);
	frames = (u32int *)kmalloc(INDEX_FROM_BIT(nframes) * 4);
	memset(frames, 0, INDEX_FROM_BIT(nframes) * 4);

	printk(KPL_DUMP, "Required are 0x%x frames (allocated 0x%x at 0x%x)\n", mem_end_page / 0x1000, INDEX_FROM_BIT(nframes) * 32, frames);

	kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));
	current_directory = kernel_directory;

	u32int i = 0;
	while (i < mem_end_page)
	{
		alloc_new_page(get_page(i, 1, kernel_directory), 1, 1);
		i += 0x1000;
	}

	switch_page_directory(kernel_directory);
}
