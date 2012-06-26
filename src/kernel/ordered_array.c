#include <kernel/mem.h>
#include <mm/kheap.h>
#include <kernel/printk.h>
#include <ordered_array.h>

s8int standard_lessthan_predicate(void *a, void *b)
{
	return (a < b) ? 1 : 0;
}

ordered_array_t create_ordered_array(u32int max_size, lessthan_predicate_t less_than)
{
	ordered_array_t to_return;
	to_return.array = (void *)kmalloc(max_size * sizeof(void *));
	memset(to_return.array, 0, max_size * sizeof(void *));
	to_return.size = 0;
	to_return.max_size = max_size;
	to_return.less_than = less_than;

	return to_return;
}

ordered_array_t place_ordered_array(void *addr, u32int max_size, lessthan_predicate_t less_than)
{
	ordered_array_t to_ret;
	to_ret.array = (void **)addr;
	memset(to_ret.array, 0, max_size * sizeof(void *));
	to_ret.size = 0;
	to_ret.max_size = max_size;
	to_ret.less_than = less_than;

	return to_ret;
}


void destroy_ordered_array(ordered_array_t *array)
{
	// kfree(array->array);
}

void insert_ordered_array(void *item, ordered_array_t *array)
{
	if (array->less_than == NULL)
	{
		printk(KPL_PANIC, "%s:%s BROKEN ARRAY: no less_than\n", __FILE__, __LINE__);
		BOCHS_DEBUG();
	}

	u32int iterator = 0;
	while (iterator < array->size && array->less_than(array->array[iterator], item))
		iterator++;

	if (iterator == array->size)
		array->array[array->size++] = item;
	else
	{
		void *tmp = array->array[iterator];
		array->array[iterator] = item;
		
		while (iterator < array->size)
		{
			iterator++;
			void *tmp2 = array->array[iterator];
			array->array[iterator] = tmp;
			tmp = tmp2;
		}

		array->size++;
	}
}

void* lookup_ordered_array(u32int i, ordered_array_t *array)
{
	if (i >= array->size)
	{
		printk(KPL_PANIC, "%s:%s bad lookup index %d\n", __FILE__, __LINE__, i);
		BOCHS_DEBUG();
	}

	return array->array[i];
}

void remove_ordered_array(u32int i, ordered_array_t *array)
{
	while (i < array->size)
	{
		array->array[i] = array->array[i + 1];
		i++;
	}

	array->size--;
}

