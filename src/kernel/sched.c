#include "kernel/sched.h"
#include <kernel/mem.h>
#include <mm/paging.h>
#include <kernel/printk.h>

u32int user_stack[PAGE_SIZE >> 2];
struct {
	u32int *a;
	u16int b;
} stack_start = { &user_stack[PAGE_SIZE >> 2], 0x10 };

static u32int task0_stack0[PAGE_SIZE >> 2];

extern page_directory_t *kernel_directory;

struct task_struct task0 = {
	/* tss */
	{ 0, (u32int)task0_stack0 + sizeof(task0_stack0), DATA_SEL,
		0,0,0,0,
		(u32int)&pg_dir,
		0,0,0,0,0,0,0,0,0,0,
		USER_DATA_SEL, USER_CODE_SEL, USER_DATA_SEL, USER_DATA_SEL,
		USER_DATA_SEL, USER_DATA_SEL,
		LDT(0),
		0x00
	},
	{ 0, 0 }, /* tss_entry */
	INITIAL_PRIO, /* priority */
	TS_RUNNING, /* state */
	/* ldt: 0-code; 1-data */
	{
		{ 0xffff, 0xcf9a00 },
		{ 0xffff, 0xcf9200 }
	},
	{ 0, 0 }, /* ldt_entry */
	0 /* next */
};

struct task_struct *current = &task0;

extern void task1_run(void);
extern void task2_run(void);

static u32int task1_stack0[1024];
static u32int task1_stack3[1024];
static u32int task2_stack0[1024];
static u32int task2_stack3[1024];

struct task_struct task1;
struct task_struct task2;

static void new_task(struct task_struct *task, u32int eip,
		u32int sp0, u32int sp3)
{
	memcpy(&task0, task, sizeof(struct task_struct));
	task->tss.esp0 = sp0;
	task->tss.eip = eip;
	task->tss.esp = sp3;
	task->tss.eflags = 0x3202;

	task->priority = INITIAL_PRIO;

	task->state = TS_STOPPED;
	task->next = current->next;
	current->next = task;
	task->state = TS_RUNABLE;
}

extern void do_task1(void);
extern void do_task2(void);

void task1_work(void)
{
	int i;
	printk(KPL_DUMP, "A");
	for (i = 0; i < 1000; ++i)
		__asm__ __volatile__("nop");
}

void task2_work(void)
{
	int i;
	printk(KPL_DUMP, "B");

	for (i = 0; i < 1000; ++i)
		__asm__ __volatile__("nop");
}

#define set_tss_desc(n, addr) set_tss_ldt_desc((char *)(n), addr, 0x89)
#define set_ldt_desc(n, addr) set_tss_ldt_desc((char *)(n), addr, 0x82)
struct desc_struct set_tss_ldt_desc(char *gdt_dest, u32int addr, u8int type)
{
	*gdt_dest = 0x68; /* limit field - 104 bytes */
	*(gdt_dest+2) = (addr & 0xff); /* base 15:00 */
	*(gdt_dest+3) = (addr >> 8) & 0xff;
	*(gdt_dest+4) = (addr >> 16) & 0xff; /* base 16:23 */
	*(gdt_dest+5) = type;
	*(gdt_dest+6) = 0x00;
	*(gdt_dest+7) = (addr >> 24) & 0xff; /* base 24:31 */

	return *((struct desc_struct*)gdt_dest);
}

struct desc_struct get_tss(void)
{
	return gdt[FIRST_TSS_SEL];
}

struct desc_struct get_ldt(void)
{
	return gdt[FIRST_LDT_SEL];
}

void schedule(void)
{
	struct task_struct *v = &task0, *tmp = 0;
	int cp = current->priority;

	for (; v; v = v->next)
	{
		if ((v->state == TS_RUNABLE) && (cp > v->priority))
		{
			tmp = v;
			cp = v->priority;
		}
	}

	if (tmp && (tmp != current))
	{
		current->tss_entry = get_tss();
		current->ldt_entry = get_ldt();
		tmp->tss_entry = set_tss_desc(gdt + FIRST_TSS_SEL, (u32int)&tmp->tss);
		tmp->ldt_entry = set_ldt_desc(gdt + FIRST_LDT_SEL, (u32int)&tmp->ldt);
		current->state = TS_RUNABLE;
		tmp->state = TS_RUNNING;
		current = tmp;
		__asm__ __volatile__ ("ljmp $0x20, $0\n\t");
	}
}

void sched_init(void)
{
	task0.tss.cr3 = (u32int)kernel_directory->tablesPhysical;

	set_tss_desc(gdt + FIRST_TSS_SEL, (u32int)&task0.tss);
	set_ldt_desc(gdt + FIRST_LDT_SEL, (u32int)&task0.ldt);

	__asm__("ltrw %%ax\n\t" ::"a"(TSS(0)));
	__asm__("lldt %%ax\n\t" ::"a"(LDT(0)));
	new_task(&task1, (u32int)do_task1,
			(u32int)task1_stack0 + sizeof task1_stack0, (u32int)task1_stack3 + sizeof task1_stack3);
	new_task(&task2, (u32int)do_task2,
			(u32int)task2_stack0 + sizeof task2_stack0, (u32int)task2_stack3 + sizeof task2_stack3);
}

