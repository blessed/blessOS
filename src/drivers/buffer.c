/* This file contains the buffer cache logic as described
 * in Maurice Bach's book on the Unix operating system */

#include <kernel/common.h>
#include <kernel/buffer.h>
#include <kernel/printk.h>
#include <kernel/system.h>
#include <kernel/sched.h>

buffer_head_t *hash_table[HASH_MOD];
buffer_head_t *free_list;

static inline u32int hash(s32int dev, s32int block)
{
	return ((dev ^ block) % HASH_MOD);
}

static inline buffer_head_t* find_in_hashtable(int dev, int block)
{
	return NULL;
}

static void remove_from_lists(buffer_head_t *buf)
{
	/* first remove the buffer from the hash list */
	if (buf->next)
		buf->next->prev = buf->prev;
	if (buf->prev)
		buf->prev->next = buf->next;

	if (hash_table[hash(buf->dev, buf->block_no)] == buf)
		hash_table[hash(buf->dev, buf->block_no)] = buf->next;

	/* now we remove the buffer from free list */
	if (!(buf->free_prev) || !(buf->free_next))
	{
		printk(KPL_PANIC, "Buffer is corrupted!\n");
		halt();
	}

	buf->free_prev->free_next = buf->free_next;
	buf->free_next->free_prev = buf->free_prev;
	if (free_list == buf)
		free_list = buf->free_next;
}

buffer_head_t* getblk(s32int dev, s32int block)
{
	buffer_head_t *buf = NULL;

	while (!buf)
	{
		if ((buf = find_in_hashtable(dev, block)))
		{
			if (buf->lock)
			{
				sleep_on(&buf->task_wait);
				continue;
			}

			buf->lock = 1;
			buf->count++;
			remove_from_lists(buf);
			
			buf->dev = dev;
			buf->block_no = block;
			buf->dirt = 0;
			buf->uptodate = 0;
			return buf;
		}
	}

	return buf;
}

