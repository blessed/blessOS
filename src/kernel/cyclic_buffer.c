#include <kernel/cyclic_buffer.h>
#include <kernel/mem.h>
#include <kernel/printk.h>

void cb_init(circular_buf_t *buf)
{
	memset(buf->data, 0, sizeof(buf->data));
	buf->head = 0;
	buf->tail = 0;
}

s8int cb_push(u8int *element, circular_buf_t *buf)
{
	if (cb_is_full(buf) == 1)
	{
		printk(KPL_DUMP, "Buffer is full head = %d, tail = %d\n", buf->head, buf->tail);
		return -1;
	}

	buf->data[buf->head++] = *element;

	return 0;
}

u8int cb_pop(circular_buf_t *buf)
{
	if (!buf)
	{
		printk(KPL_PANIC, "%s:%s: buffer is NULL\n", __FILE__, __LINE__);
		return (u8int)-1;
	}

	if (cb_is_empty(buf))
		return NULL;

	return buf->data[buf->tail++];
}

int cb_is_full(circular_buf_t *buf)
{
	if (!buf)
	{
		printk(KPL_PANIC, "%s:%s: buffer is NULL\n", __FILE__, __LINE__);
		return -1;
	}

	if ((buf->tail - buf->head - 1) & (CIRC_BUFF_SIZE - 1))
		return 0;
	else
		return 1;
}

int cb_is_empty(circular_buf_t *buf)
{
	if (!buf)
		return -1;

	if (buf->head == buf->tail)
		return 1;
	else
		return 0;
}

int cb_get_count(circular_buf_t *buf)
{
	if (!buf)
		return -1;

	return ((buf->head - buf->tail) & (CIRC_BUFF_SIZE - 1));
}

