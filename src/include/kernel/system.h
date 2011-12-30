#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/* sets a gate at gate_addr with given parameters */
#define set_gate(gate_addr, type, dpl, addr) \
	asm("movw %%dx, %%ax\n\t" \
			"movw %0, %%dx\n\t" \
			"movl %%eax, %1\n\t" \
			"movl %%edx, %2\n\r" \
			: \
			: "i" ((short)(0x8000 + (dpl << 13) + (type << 8))), \
			"o" (*((char *)(gate_addr))), \
			"o" (*(4+(char*)(gate_addr))), \
			"d" ((char *)(addr)), "a" (0x00080000))

#define set_trap_gate(n, addr) \
	set_gate(&idt[n], 15, 0, addr)

#define set_intr_gate(n, addr) \
	set_gate(&idt[n], 14, 0, addr)

#define set_system_gate(n, addr) \
	set_gate(&idt[n], 15, 3, addr)

#define halt() asm("cli; hlt\t\n");
#define idle() asm("jmp .\t\n");

#define cli() asm("cli \t\n");
#define sti() asm("sti \t\n");

typedef struct desc_struct
{
	unsigned long a, b;
} desc_table[256];

extern desc_table idt, gdt;
extern unsigned int pg_dir[1024];

#define FIRST_TSS_SEL 4
#define FIRST_LDT_SEL (FIRST_TSS_SEL + 1)

#define DATA_SEL 0x10
#define CODE_SEL 0x08

#define USER_DATA_SEL 0x0C
#define USER_CODE_SEL 0x04

#define LDT(n) ((((u32int)(n)) << 4) + (FIRST_LDT_SEL << 3))
#define TSS(n) ((((u32int)(n)) << 4) + (FIRST_TSS_SEL << 3))

#define PAGE_SIZE 4096

#endif /* _SYSTEM_H_ */
