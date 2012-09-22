#ifndef CYCLIC_BUFFER_H
#define CYCLIC_BUFFER_H

#include <kernel/common.h>

#define CIRC_BUFF_SIZE 1024

typedef struct circular_buf
{
	u8int data[CIRC_BUFF_SIZE];
	u32int head;
	u32int tail;
} circular_buf_t;

void cb_init(circular_buf_t *buf);
s8int cb_push(u8int element, circular_buf_t *buf);
u8int cb_pop(circular_buf_t *buf);
void cb_clear(circular_buf_t *buf);
int cb_is_full(circular_buf_t *buf);
int cb_is_empty(circular_buf_t *buf);
int cb_get_count(circular_buf_t *buf);

#endif /* CYCLIC_BUFFER_H */

