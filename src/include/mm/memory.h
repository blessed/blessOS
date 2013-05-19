#ifndef MEMORY_H
#define MEMORY_H

typedef struct page
{
	u32int present		: 1;
	u32int rw			: 1;
	u32int user			: 1;
	u32int accessed		: 1;
	u32int dirty		: 1;
	u32int unused		: 7;
	u32int frame		: 20;
} page_t;

typedef struct page_table
{
	page_t *page[1024];
} page_table_t;

typedef struct page_dir
{
	page_table_t 	*table[1024];
	u32int			table_phys_addr[1024];

	u32int			dir_phys_addr;
} page_dir_t;

u32int* get_free_page(void);
u32int put_page(u32int page, u32int address);
void remove_page(u32int address);
u32int* get_page_addr(u32int address);
void no_page(u32int error, u32int address);
void switch_page_dir(page_dir_t *dir);
void free_page(u32int address);
int free_page_tables(u32int from, u32int size);

#endif /* MEMORY_H */
