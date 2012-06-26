#ifndef PAGING_H
#define PAGING_H

#include <kernel/common.h>

typedef struct page
{
	u32int present	: 1;
	u32int rw		: 1;
	u32int user		: 1;
	u32int accessed	: 1;
	u32int dirty	: 1;
	u32int unused	: 7;
	u32int frame	: 20;
} page_t;

typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	page_table_t *tables[1024];
	u32int tablesPhysical[1024];

	u32int physicalAddr;
} page_directory_t;

void initialise_paging(void);

void switch_page_directory(page_directory_t *new_dir);

page_t* get_page(u32int address, u8int make, page_directory_t *dir);

#endif /* PAGING_H */
