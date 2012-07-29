#ifndef CONFIG_H
#define CONFIG_H

#define HZ 100

#define HIGH_MEM	0x800000 /* 8 MiB currently */
#define LOW_MEM		0x100000 /* 0-1MiB is for the kernel and BIOS data area */

#define PAGE_SIZE	4096
#define PAGE_TABLE_ENTRIES_COUNT 1024

#define PAGING_MEMORY (HIGH_MEM - LOW_MEM)
#define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

#endif /* CONFIG_H */
