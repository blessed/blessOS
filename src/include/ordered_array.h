#ifndef ORDERED_TREE_H
#define ORDERED_TREE_H

#include <kernel/common.h>

typedef s8int (*lessthan_predicate_t)(void*, void*);
typedef struct
{
	void **array;
	u32int size;
	u32int max_size;
	lessthan_predicate_t less_than;
} ordered_array_t;

s8int standard_lessthan_predicate(void *a, void *b);

ordered_array_t create_ordered_array(u32int max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void *addr, u32int max_size, lessthan_predicate_t less_than);

void destroy_ordered_array(ordered_array_t *array);

void insert_ordered_array(void *item, ordered_array_t *array);

void* lookup_ordered_array(u32int i, ordered_array_t *array);

void remove_ordered_array(u32int i, ordered_array_t *array);

#endif /* ORDERED_TREE_H */
