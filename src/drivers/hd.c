/* hd.c
 * Contains the hard disk interrupt routines and all code related
 * to hard disk request queues. */

#include <kernel/common.h>
#include <kernel/system.h>
#include <mm/kheap.h>
#include <drivers/hd.h>
#include <drivers/pata.h>
#include <kernel/printk.h>
#include <kernel/exceptions.h>

request_queue_t executing_requests = { 0, NULL, NULL };
request_queue_t pending_requests = { 0, NULL, NULL };
request_queue_t completed_requests = { 0, NULL, NULL };

void hd_interrupt(void)
{
	printk(KPL_DUMP, "Caught a HD interrupt\n");

	hd_request_t *tmp = executing_requests.head;

	printk(KPL_DUMP, "The completed request is:\n");
	printk(KPL_DUMP, "drive %d, sect count %d, sector %d, head %d, cyl %d, type %d",
			tmp->hd, tmp->nsector, tmp->sector, tmp->head, tmp->cyl, tmp->type);

	while (--tmp->nsector)
	{
		write_to_port(tmp->bh->block_data, ATA_PRI_DATA, 256);
		return;
	}

	EOI();

	/* TODO: Need to be finished, bh updated and furhter request submitted */
}

hd_request_t* get_request(void)
{
	hd_request_t *p;

	for (p = requests; p < requests+HD_REQUESTS_LIMIT; ++p)
	{
		if (p->hd == 0)
			return p;
	}

	return NULL;
}

int is_queue_empty(request_queue_t *queue)
{
	if (queue->head == NULL && queue->tail == NULL)
	{
		return 1;
	}

	return 0;
}

hd_request_t* create_request(int hd, int nsector, int sector, int head, int cylinder, hd_request_type_t type,
		struct buffer_head *bh)
{
	/*
	if (!req)
	{
		printk(KPL_DUMP, "Bad request token\n");
		return;
	}
	*/

	hd_request_t *req;

	printk(KPL_DUMP, "Creating request\n");

	req = (hd_request_t *)kmalloc(sizeof(hd_request_t));
	if (!req)
	{
		printk(KPL_PANIC, "Couldn't allocate memory for request\n");
		halt();
	}

	req->hd = hd;
	req->nsector = nsector;
	req->sector = sector;
	req->head = head;
	req->cyl = cylinder;
	req->type = type;
	req->bh = bh;
	req->errors = 0;
	req->next = NULL;

	return req;
}

int add_request_to_queue(request_queue_t *queue, hd_request_t *req)
{
	printk(KPL_DUMP, "Adding request %p to queue %p\n", req, queue);

	if (!queue)
	{
		printk(KPL_PANIC, "Invalid queue as parameter\n");
		return -1;
	}

	req->next = NULL; /* reset its previous reference */
	if (is_queue_empty(queue))
	{
		queue->head = queue->tail = req;
		queue->size++;
	}
	else
	{
		queue->tail->next = req;
		queue->tail = req;
		queue->size++;
	}

	return 0;
}

hd_request_t* remove_request_from_queue(request_queue_t *queue)
{
	printk(KPL_DUMP, "Removing from queue %p\n", queue);

	if (!queue)
	{
		printk(KPL_DUMP, "Invalid queue\n");
		return NULL;
	}

	if (!queue->head && !queue->tail)
	{
		printk(KPL_PANIC, "List is empty\n");
		return NULL;
	}

	hd_request_t *h, *p;
	h = queue->head;
	p = h->next;
	queue->head = p;

	if (!queue->head)
		queue->tail = queue->head;

	return h;
}

int switch_queues(request_queue_t *source, request_queue_t *target)
{
	hd_request_t *tmp;

	if (!source || !target)
	{
		printk(KPL_PANIC, "Invalid parameters for switching queues\n");
		return -1;
	}

	if (is_queue_empty(source))
	{
		printk(KPL_PANIC, "Source queue is empty!\n");
		return -1;
	}

	tmp = remove_request_from_queue(source);
	if (!tmp)
	{
		printk(KPL_DUMP, "Removed bad corrupted element from queue\n");
		return -1;
	}

	add_request_to_queue(target, tmp);

	return 0;
}

void submit_request()
{
	printk(KPL_DUMP, "Submitting request to hard drive\n");

	if (switch_queues(&pending_requests, &executing_requests))
	{
		printk(KPL_PANIC, "Couldn't switch queues\n");
		halt();
	}

	//remove_request_from_queue(&executing_requests);

	hd_request_t *req;
	req = executing_requests.head;

	printk(KPL_DUMP, "%p %x\n", req, req->type == HD_REQUEST_TYPE_READ ? ATA_CMD_READ : ATA_CMD_WRITE);

	printk(KPL_DUMP, "hd %d, nsector %d, sector %d, head %d, cyl %d, type %d\n", req->hd, req->nsector, req->sector,
			req->head, req->cyl, req->type);
	hd_rw(req->hd, req->nsector, req->sector, req->head, req->cyl, req->type == HD_REQUEST_TYPE_READ ? ATA_CMD_READ : ATA_CMD_WRITE);

	if (HD_REQUEST_TYPE_WRITE == req->type)
	{
		if (is_hd_drq())
		{
			write_to_port(req->bh->block_data, ATA_PRI_DATA, 256);
		}
	}
}
