#ifndef _DESCRIPTORS__INCLUDED_
#define _DESCRIPTORS__INCLUDED_

/* descriptor data structure */
typedef struct desc_struct
{
	unsigned long a, b;
} desc_table[256];

/* both gdt and idt are defined in setup.s where they are created */
extern desc_table gdt, idt;

/* page directory of size 4 * 1024, see at setup.s */
extern unsigned long pg_dir[1024];

#define GDT_NULL	0
#define GDT_CODE	1
#define GDT_DATA	2
#define GDT_TMP		3

#define LDT_NULL	0
#define LDT_CODE	1
#define LDT_DATA	2

#endif /* _DESCRIPTORS__INCLUDED_*/
