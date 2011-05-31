#include <asm.h>
#include <kprintf.h>
#include <scr.h>

void
divide_error(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
debug_exception(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
breakpoint(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
nmi(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
overflow(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
bounds_check(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
invalid_opcode(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
cop_not_avalid(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
double_fault(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
overrun(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
invalid_tss(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
seg_not_present(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
stack_exception(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
general_protection(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
page_fault(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
reversed(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}

void
coprocessor_error(void) {
	asm ("pushl	%%eax;call	info"::"a"(KPL_PANIC));
	halt();
}


