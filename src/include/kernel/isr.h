#ifndef _ISR_H_
#define _ISR_H_

#include <kernel/common.h>

#define VALID_ISR (32)
#define INTERRUPT_COUNT (1)

typedef struct intr_stack_frame
{
	u32int ss;
	u32int gs;
	u32int fs;
	u32int es;
	u32int ds;
	u32int edi;
	u32int esi;
	u32int ebp;
	u32int esp;
	u32int ebx;
	u32int edx;
	u32int ecx;
	u32int eax;
	u32int isr_nr;
	u32int err;
	u32int eip;
	u32int cs;
	u32int eflags;
	u32int old_esp;
	u32int old_ss;
} intr_stack_frame_t;

void default_isr(void);

extern void (*isr_table[(VALID_ISR + INTERRUPT_COUNT) << 1])(void);

#endif /* _ISR_H_ */
