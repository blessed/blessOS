#include <mm/paging.h>
#include <mm/kheap.h>
#include <kernel/mem.h>

page_directory_t *kernel_directory = 0;

page_directory_t *current_dir = 0;

u32int *frames;
u32int nframes;

extern u32int placement_address;

#define INDEX_FROM_BIT(a) ((a) / (8*4))
#define OFFSET_FROM_BIT(a) ((a) % (8*4))

static void set_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x01 << off);
}

static void clear_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x01 << off);
}

static u32int test_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x01 << off));
}

static u32int first_frame(void)
{
	u32int i, j;

	for (i = 0; i < INDEX_FROM_BIT(nframes); ++i)
	{
		if (frames[i] != 0xFFFFFFFF)
		{
			for (j = 0; j < 32; ++j)
			{
				u32int toTest = 0x01 << j;
				if (!(frames[i] & toTest))
				{
					return i * 4 * 8 + j;
				}
			}
		}
	}

	return (u32int)-1;
}

void alloc_frame(page_t *page, int is_kernel, int rw)
{
	if (page->frame != 0)
	{
		return;
	}
	else
	{
		u32int idx = first_frame();
		if (idx == (u32int)-1)
		{
			/* TODO: this place should actually PANIC */
			while (1)
				asm volatile("nop");
		}
		set_frame(idx * 0x1000);
		page->present = 1;
		page->rw = (rw) ? 1 : 0;
		page->user = (is_kernel) ? 0 : 1;
		page->frame = idx;
	}
}

void free_frame(page_t *page)
{
	u32int frame;

	if (!(frame = page->frame))
	{
		return;
	}
	else
	{
		clear_frame(frame);
		page->frame = 0;
	}
}

void initialize_paging(void)
{
	u32int mem_end_page = 0x800000;
	
	nframes = mem_end_page / 0x1000;
	frames = (u32int *)kmalloc(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));
	
	kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
	current_dir = kernel_directory;

	int i = 0;
	while (i < placement_address)
	{
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;
	}
	
	switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir)
{
	current_dir = dir;
	asm volatile ("mov %0, %%cr3" :: "r"(&dir->tablesPhysical));
	u32int cr0;
	asm volatile ("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" :: "r"(cr0));
}

page_t *get_page(u32int address, int make, page_directory_t *dir)
{
	address /= 0x1000;
	u32int table_idx = address / 1024;
	if (dir->tables[table_idx])
	{
		return (&dir->tables[table_idx]->pages[address % 1024]);
	}
	else if (make)
	{
		u32int tmp;
		dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
		dir->tablesPhysical[table_idx] = tmp | 0x07;
		return &dir->tables[table_idx]->pages[address % 1024];
	}
	else
		return 0;
}
