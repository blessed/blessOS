#ifndef HD_H
#define HD_H

#include <kernel/buffer.h>
#include <kernel/common.h>

#define HD_REQUESTS_LIMIT 10

typedef enum HD_REQUEST_TYPE
{
	HD_REQUEST_TYPE_READ,
	HD_REQUEST_TYPE_WRITE
} hd_request_type_t;

struct hd_request
{
	u32int hd;
	u32int nsector;
	u32int sector;
	u32int head;
	u32int cyl;
	hd_request_type_t type;
	u32int errors;
	struct buffer_head *bh;
	struct hd_request *next;
} requests[HD_REQUESTS_LIMIT];

typedef struct hd_request hd_request_t;

typedef struct request_queue
{
	u32int size;	
	hd_request_t *head;
	hd_request_t *tail;
} request_queue_t;

extern request_queue_t executing_requests;
extern request_queue_t pending_requests;
extern request_queue_t completed_requests;

void hd_interrupt(void);

hd_request_t* get_request(void);
int is_queue_empty(request_queue_t *queue);
hd_request_t* create_request(int hd, int nsector, int sector, int head, int cylinder, hd_request_type_t type,
		struct buffer_head *bh);

int add_request_to_queue(request_queue_t *queue, hd_request_t *req);
int switch_queues(request_queue_t *source, request_queue_t *target);
hd_request_t* remove_request_from_queue(request_queue_t *queue);

/* This function submits the request that is at the first request in queue. It shall be called
 * everytime there is a request completed or there is a request created. */
void submit_request();

#endif /* HD_H */
