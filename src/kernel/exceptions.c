#include <kernel/system.h>
#include <kernel/console.h>
#include <kernel/printk.h>
#include <kernel/isr.h>
#include <asm/io.h>

void divide_error(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}

void debug(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void nmi(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void breakpoint(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void overflow(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void bounds_check(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void invalid_opcode(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void coprocessor_not_av(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void double_fault(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void invalid_TSS(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void segment_not_present(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void stack_exception(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void general_protection(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void page_fault(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void coprocessor_error(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}
void reserved(void)
{
	asm("pushl %%eax; call info"::"a"(KPL_PANIC));
	halt();
}

void info(enum KP_LEVEL kl,
		unsigned int ret_ip, unsigned int ss, unsigned int gs, unsigned int fs,
		unsigned int es, unsigned int ds, unsigned int edi, unsigned int esi,
		unsigned int ebp, unsigned int esp, unsigned int ebx, unsigned int edx,
		unsigned int ecx, unsigned int eax, unsigned int isr_nr, unsigned int err,
		unsigned int eip, unsigned int cs, unsigned int eflags,
		unsigned int old_esp, unsigned int old_ss)
{
	static const char *exception_msg[] = {
		"DIVIDE ERROR",
		"DEBUG EXCEPTION",
		"BREAKPOINT",
		"NMI",
		"OVERFLOW",
		"BOUNDS CHECK",
		"INVALID OPCODE",
		"COPROCESSOR NOT VALID",
		"DOUBLE FAULT",
		"OVERRUN",
		"INVALID TSS",
		"SEGMENTATION NOT PRESENT",
		"STACK EXCEPTION",
		"GENERAL PROTECTION",
		"PAGE FAULT",
		"RESERVED",
		"COPROCESSOR_ERROR",
	};
	unsigned int cr2, cr3;
	(void)ret_ip;
	asm volatile ("movl	%%cr2,	%%eax":"=a"(cr2));
	asm volatile ("movl %%cr3,	%%eax":"=a"(cr3));
	if (isr_nr < sizeof(exception_msg))
		printk(kl, "EXCEPTION %d: %s\n=======================\n",
				isr_nr, exception_msg[isr_nr]);
	else
		printk(kl, "INTERRUPT %d\n=======================\n", isr_nr);
	printk(kl, "cs:\t%x\teip:\t%x\teflags:\t%x\n", cs, eip, eflags);
	printk(kl, "ss:\t%x\tesp:\t%x\n", ss, esp);
	printk(kl, "old ss:\t%x\told esp:%x\n", old_ss, old_esp);
	printk(kl, "errcode:%x\tcr2:\t%x\tcr3:\t%x\n", err, cr2, cr3);
	printk(kl, "General Registers:\n=======================\n");
	printk(kl, "eax:\t%x\tebx:\t%x\n", eax, ebx);
	printk(kl, "ecx:\t%x\tedx:\t%x\n", ecx, edx);
	printk(kl, "esi:\t%x\tedi:\t%x\tebp:\t%x\n", esi, edi, ebp);
	printk(kl, "Segment Registers:\n=======================\n");
	printk(kl, "ds:\t%x\tes:\t%x\n", ds, es);
	printk(kl, "fs:\t%x\tgs:\t%x\n", fs, gs);
}

void EOI(void)
{
	outb(0x20, 0x20);
}

void traps_init(void)
{
	int i;

	for (i = 0; i < VALID_ISR+2; ++i)
		set_intr_gate(i, (unsigned int)isr_table[(i << 1) + 1]);
	for (; i < 256; ++i)
		set_intr_gate(i, (unsigned int)isr_table[(i << 1) + 1]);
}
