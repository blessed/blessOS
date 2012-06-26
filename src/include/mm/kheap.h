#ifndef KHEAP_H
#define KHEAP_H

#include <kernel/common.h>

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

#endif /* KHEAP_H */
