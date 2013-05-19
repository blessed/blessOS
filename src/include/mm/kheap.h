#ifndef KHEAP_H
#define KHEAP_H

#include <kernel/common.h>
#include <kernel/ordered_array.h>

/* Allocates memory with the ability to align it to page boundary & returning it's physical address*/
u32int kmalloc_int(u32int sz, int align, u32int *phys);

/* Allocate page aligned memory */
u32int kmalloc_a(u32int sz);

/* Allocate new memory and return it's physical address */
u32int kmalloc_p(u32int sz, u32int *phys);

/* Allocate new aligned memory and return it's physical address */
u32int kmalloc_ap(u32int sz, u32int *phys);

/* Allocate new memory */
u32int kmalloc(u32int sz);

#define KHEAP_START				0x300000
#define KHEAP_INITIAL_SIZE		0x100000

#define HEAP_INDEX_SIZE		0x20000
#define HEAP_MAGIC				0x123890AB
#define HEAP_MIN_SIZE			0x8000

typedef struct
{
	u32int magic;
	u8int is_hole;
	u32int size;
} header_t;

typedef struct
{
	u32int magic;
	header_t *header;
} footer_t;

typedef struct
{
	ordered_array_t index;
	u32int start_address;
	u32int end_address;
	u32int max_address;
	u8int  supervisor;
	u8int readonly;
} heap_t;

heap_t* create_heap(u32int start, u32int end, u32int max);
void* alloc(u32int size, u8int page_align, heap_t *heap);
void free(void *p, heap_t *heap);

#endif /* KHEAP_H */
