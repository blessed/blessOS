#include <mm/kheap.h>
#include <mm/memory.h>
#include <kernel/printk.h>

extern u32int end;
u32int placement_address = (u32int)&end;
heap_t *kheap = NULL;

/*
u32int kmalloc_int(u32int sz, int align, u32int *phys)
{
	if (align && (placement_address & ~0xFFFFF000))
	{
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}

	if (phys)
	{
		*phys = placement_address;
	}

	u32int tmp = placement_address;
	placement_address += sz;

	return tmp;
}

u32int kmalloc_a(u32int sz)
{
	return kmalloc_int(sz, 1, 0);
}

u32int kmalloc_p(u32int sz, u32int *phys)
{
	return kmalloc_int(sz, 0, phys);
}

u32int kmalloc_ap(u32int sz, u32int *phys)
{
	return kmalloc_int(sz, 1, phys);
}

u32int kmalloc(u32int sz)
{
	return kmalloc_int(sz, 0, 0);
}*/

u32int kmalloc_int(u32int sz, int align, u32int *phys)
{
	if (kheap)
	{
		void *addr = alloc(sz, (u8int)align, kheap);
		if (phys)
		{
			u32int *page = get_page_addr((u32int)addr);
			*phys = (u32int)page;
		}
		return (u32int)addr;
	}
	else
	{
		if (align == 1 && (placement_address & ~0xFFFFF000))
		{
			placement_address &= 0xFFFFF000;
			placement_address += 0x1000;
		}

		if (phys)
			*phys = placement_address;
		
		u32int tmp = placement_address;
		placement_address += sz;

		return tmp;
	}
}

u32int kmalloc_a(u32int sz)
{
	return kmalloc_int(sz, 1, NULL);
}

u32int kmalloc_p(u32int sz, u32int *phys)
{
	return kmalloc_int(sz, 0, phys);
}

u32int kmalloc_ap(u32int sz, u32int *phys)
{
	return kmalloc_int(sz, 1, phys);
}

u32int kmalloc(u32int sz)
{
	return kmalloc_int(sz, 0, NULL);
}

void kfree(void *p)
{
	free(p, kheap);
}

static void expand(u32int new_size, heap_t *heap)
{
	ASSERT(new_size > heap->end_address - heap->start_address);

	if (new_size & 0xfffff000)
	{
		new_size &= 0xfffff000;
		new_size += 0x1000;
	}

	ASSERT(heap->start_address + new_size <= heap->max_address);

	u32int old_size = heap->end_address - heap->start_address;

	u32int i = old_size;
	while (i < new_size)
	{
		if (!get_page_addr(heap->start_address + i))
			put_page((u32int)get_free_page(), heap->start_address + i);

		i += 0x1000;
	}

	heap->end_address = heap->start_address + new_size;
}

static u32int contract(u32int new_size, heap_t *heap)
{
	ASSERT(new_size < heap->end_address - heap->start_address);

	if (new_size & 0xfffff000)
	{
		new_size &= 0xfffff000;
		new_size += 0x1000;
	}

	if (new_size < HEAP_MIN_SIZE)
		new_size = HEAP_MIN_SIZE;

	u32int old_size = heap->end_address - heap->start_address;
	u32int i = old_size - 0x1000;
	while (new_size < i)
	{
		//free_page((u32int)get_page_addr(heap->start_address + i));
		remove_page(heap->start_address + i);
		i -= 0x1000;
	}

	heap->end_address = heap->start_address + new_size;
	return new_size;
}

static u32int find_smallest_hole(u32int size, u8int page_align, heap_t *heap)
{
	u32int iterator = 0;
	while (iterator < heap->index.size)
	{
		header_t *header = (header_t *)lookup_ordered_array(iterator, &heap->index);
		if (page_align)
		{
			u32int location = (u32int)header;
			s32int offset = 0;
			if ((location + sizeof(header_t)) & ~0xfffff000)
			{
				offset = 0x1000 - (location + sizeof(header_t)) % 0x1000;
			}
			s32int hole_size = (s32int)header->size - offset;

			if (hole_size >= (s32int)size)
				break;
		}
		else if (header->size >= size)
			break;

		iterator++;
	}

	if (iterator == heap->index.size)
		return -1;
	else
		return iterator;
}

static s8int header_less_than(void *a, void *b)
{
	return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

heap_t* create_heap(u32int start, u32int end, u32int max)
{
	heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

	ASSERT(start % 0x1000 == 0);
	ASSERT(end % 0x1000 == 0);

	heap->index = place_ordered_array((void *)start, HEAP_INDEX_SIZE, header_less_than);

	start += sizeof(type_t) * HEAP_INDEX_SIZE;

	if (start & ~0xfffff000)
	{
		start &= 0xfffff000;
		start += 0x1000;
	}

	heap->start_address = start;
	heap->end_address = end;
	heap->max_address = max;

	header_t *hole = (header_t *)start;
	hole->size = end - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_ordered_array((type_t)hole, &heap->index);

	return heap;
}

void* alloc(u32int size, u8int page_align, heap_t *heap)
{
	u32int new_size = size + sizeof(header_t) + sizeof(footer_t);
	s32int iterator = find_smallest_hole(new_size, page_align, heap);

	// no suitable hole found
	if (iterator == -1)
	{
		u32int old_length = heap->end_address - heap->start_address;
		u32int old_end_address = heap->end_address;

		expand(old_length + new_size, heap);
		u32int new_length = heap->end_address - heap->start_address;

		iterator = 0;
		u32int idx = -1; u32int value = 0x0;
		while ((u32int)iterator < heap->index.size)
		{
			u32int tmp = (u32int)lookup_ordered_array(iterator, &heap->index);
			if (tmp > value)
			{
				value = tmp;
				idx = iterator;
			}

			iterator++;
		}

		if ((s32int)idx == -1)
		{
			header_t *header = (header_t *)old_end_address;
			header->magic = HEAP_MAGIC;
			header->size = new_length - old_length;
			header->is_hole = 1;
			footer_t *footer = (footer_t *)(old_end_address + header->size - sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
			insert_ordered_array((void *)header, &heap->index);
		}
		else
		{
			header_t *header = lookup_ordered_array(idx, &heap->index);
			header->size += new_length - old_length;
			footer_t *footer = (footer_t *)((u32int)header + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}

		return alloc(size, page_align, heap);
	}

	header_t *orig_hole_header = (header_t *)lookup_ordered_array(iterator, &heap->index);
	u32int orig_hole_pos = (u32int)orig_hole_header;
	u32int orig_hole_size = orig_hole_header->size;

	if (orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t))
	{
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}

	if (page_align && orig_hole_pos & ~0xfffff000)
	{
		u32int new_location = orig_hole_pos + 0x1000 - (orig_hole_pos & 0xfff) - sizeof(header_t);
		header_t *hole_header = (header_t *)orig_hole_pos;
		hole_header->size = 0x1000 - (orig_hole_pos & 0xfff) - sizeof(header_t);
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		footer_t *hole_footer = (footer_t *)((u32int)new_location - sizeof(footer_t));
		hole_footer->magic = HEAP_MAGIC;
		hole_footer->header = hole_header;
		orig_hole_pos = new_location;
		orig_hole_size -= hole_header->size;
	}
	else
	{
		remove_ordered_array(iterator, &heap->index);
	}

	header_t *block_header = (header_t *)orig_hole_pos;
	block_header->magic = HEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size = new_size;
	footer_t *block_footer = (footer_t *)(orig_hole_pos + sizeof(header_t) + size);
	block_footer->magic = HEAP_MAGIC;
	block_footer->header = block_header;

	if (orig_hole_size - new_size > 0)
	{
		header_t *hole_header = (header_t *)(orig_hole_pos + sizeof(header_t) + sizeof(footer_t) + size);
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		hole_header->size = orig_hole_size - new_size;
		footer_t *hole_footer = (footer_t *)((u32int)hole_header + orig_hole_size - new_size - sizeof(footer_t));
		if ((u32int)hole_footer < heap->end_address)
		{
			hole_footer->magic = HEAP_MAGIC;
			hole_footer->header = hole_header;
		}

		insert_ordered_array((void *)hole_header, &heap->index);
	}

	return (void *)((u32int)block_header + sizeof(header_t));
}

void free(void *p, heap_t *heap)
{
	if (!p)
		return;

	header_t *header = (header_t *)((u32int)p - sizeof(header_t));
	footer_t *footer = (footer_t *)((u32int)header + header->size - sizeof(footer_t));

	ASSERT(header->magic == HEAP_MAGIC);
	ASSERT(footer->magic == HEAP_MAGIC);

	header->is_hole = 1;

	char do_add = 1;

	// Unify left
	footer_t *test_footer = (footer_t *)((u32int)header - sizeof(footer_t));
	if (test_footer->magic == HEAP_MAGIC &&
			test_footer->header->is_hole == 1)
	{
		u32int cache_size = header->size;
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		do_add = 0;
	}

	// Unify right
	header_t *test_header = (header_t *)((u32int)footer + sizeof(footer_t));
	if (test_header->magic == HEAP_MAGIC &&
			test_header->is_hole)
	{
		header->size += test_header->size;
		test_footer = (footer_t *)((u32int)test_header + test_header->size - sizeof(footer_t));

		footer = test_footer;
		u32int iterator = 0;
		while ((iterator < heap->index.size) &&
				(lookup_ordered_array(iterator, &heap->index) != (void *)test_header))
		{
			iterator++;
		}

		ASSERT(iterator < heap->index.size);
		remove_ordered_array(iterator, &heap->index);
	}

	if ((u32int)footer + sizeof(footer_t) == heap->end_address)
	{
		u32int old_length = heap->end_address - heap->start_address;
		u32int new_length = contract((u32int)header - heap->start_address, heap);

		if (header->size - (old_length - new_length) > 0)
		{
			header->size -= old_length - new_length;
			footer = (footer_t *)((u32int)header + header->size - sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
		}
		else
		{
			u32int iterator = 0;
			while ((iterator < heap->index.size) &&
					(lookup_ordered_array(iterator, &heap->index) != (void *)test_header))
			{
				iterator++;
			}

			if (iterator < heap->index.size)
				remove_ordered_array(iterator, &heap->index);
		}
	}

	if (do_add)
		insert_ordered_array((void *)header, &heap->index);
}
