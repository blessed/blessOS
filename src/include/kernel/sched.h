#ifndef SCHED_H_
#define SCHED_H_

#include "common.h"
#include "system.h"

#define TSS_SEG 0x89
#define LDT_SEG 0x82

#define INITIAL_PRIO 200

#define NR_TASKS 64

enum TASK_STATE {
	TS_RUNNING,
	TS_RUNABLE,
	TS_STOPPED
};

struct tss_struct
{
	u32int back_link; /* 16 high-bits zero */
	u32int esp0;
	u32int ss0; /* 16 hi-bits zero */
	u32int esp1;
	u32int ss1;		/* 16 hi-bits zero */
	u32int esp2;
	u32int ss2;		/* 16 hi-bits zero */
	u32int cr3;
	u32int eip;
	u32int eflags;
	u32int eax;
	u32int ecx;
	u32int edx;
	u32int ebx;
	u32int esp;
	u32int ebp;
	u32int esi;
	u32int edi;
	u32int es;
	u32int cs;
	u32int ss;
	u32int ds;
	u32int fs;
	u32int gs;
	u32int ldt;
	u32int trace_bitmap; /* bit 0-trace, 16-31 - I/O bitmap */
};

struct task_struct
{
	struct tss_struct tss;
	struct desc_struct tss_entry;
	u8int priority;
	enum TASK_STATE state;
	struct desc_struct ldt[2]; /* 0 - cs; 1 - ds && ss */
	struct desc_struct ldt_entry;
	struct task_struct *next;
};

struct desc_struct set_tss_ldt_desc(char *gdt_dest, u32int addr, u8int type);

extern struct task_struct *current;

void sched_init(void);
void schedule(void);

#endif /* SCHED_H_ */
