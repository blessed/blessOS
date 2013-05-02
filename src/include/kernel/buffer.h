#ifndef BUFFER_H
#define BUFFER_H

#include <kernel/common.h>

#define HASH_MOD 307

typedef struct buffer_head
{
	char *block_data; /* block of data */
	u16int dev; /* device number */
	u16int block_no; /* block number */
	u8int uptodate;
	struct task_struct *task_wait; /* field used for sleeping currently executing task */
	u8int dirt; /* 0 - clean, 1 - dirty */
	u8int count; /* users using this buffer */
	u8int lock; /* 0 - ok, 1 - locked */
	struct buffer_head *prev;
	struct buffer_head *next;
	struct buffer_head *free_prev;
	struct buffer_head *free_next;
} buffer_head_t;

buffer_head_t* getblk(int dev, int block);
void brelse(buffer_head_t *buf);

#endif /* BUFFER_H */
