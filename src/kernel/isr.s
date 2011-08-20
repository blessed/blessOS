/* This is the place where all the interrupts and exceptions fun is taking place
   The interrupts are redirected from here to their proper handlers
*/

.text
.globl isr_table

.macro isrnoerror nr
isr\nr:
	pushl $0
	pushl $\nr
	jmp isr_comm
.endm

.macro isrerror nr
isr\nr:
	pushl $\nr
	jmp isr_comm
.endm

isr_table:	.long divide_error, isr0x00, debug, isr0x01
			.long nmi, isr0x02, breakpoint, isr0x03
			.long overflow, isr0x04, bounds_check, isr0x05
			.long invalid_opcode, isr0x06, coprocessor_not_av, isr0x07
			.long double_fault, isr0x08, reserved, isr0x09
			.long invalid_TSS, isr0x0a, segment_not_present, isr0x0b
			.long stack_exception, isr0x0c, general_protection, isr0x0d
			.long page_fault, isr0x0e, reserved, isr0x0f
			.long coprocessor_error, isr0x10, reserved, isr0x11
			.long reserved, isr0x12, reserved, isr0x13
			.long reserved, isr0x14, reserved, isr0x15
			.long reserved, isr0x16, reserved, isr0x17
			.long reserved, isr0x18, reserved, isr0x19
			.long reserved, isr0x1a, reserved, isr0x1b
			.long reserved, isr0x1c, reserved, isr0x1d
			.long reserved, isr0x1e, reserved, isr0x1f
			.long do_timer, isr0x20, do_kb, isr0x21

/*
		+-----------+
		|  old  ss  |	76
		+-----------+
		|  old esp  |	72
		+-----------+
		|  eflags   |	68
		+-----------+
		|    cs     |	64
		+-----------+
		|   eip     |	60
		+-----------+
		|  0/err    |	56
		+-----------+
		|  isr_nr   | tmp = esp
		+-----------+
		|   eax     |	48
		+-----------+
		|   ecx     |	44
		+-----------+
		|   edx     |	40
		+-----------+
		|   ebx     |	36
		+-----------+
		|   tmp     |	32
		+-----------+
		|   ebp     |	28
		+-----------+
		|   esi     |	24
		+-----------+
		|   edi     |	20
		+-----------+
		|    ds     |	16
		+-----------+
		|    es     |	12
		+-----------+
		|    fs     |	8
		+-----------+
		|    gs     |	4
		+-----------+
		|    ss     |	0
		+-----------+
*/

isr_comm:
		pushal
		pushl	%ds
		pushl	%es
		pushl	%fs
		pushl	%gs
		pushl	%ss
		movw	$0x10,%ax
		movw	%ax,	%ds
		movw	%ax,	%es
		movw	%ax,	%fs
		movw	%ax,	%gs
		movl	52(%esp),%ecx
		call	*isr_table(, %ecx, 8)
		addl $4, %esp
		popl %gs
		popl %fs
		popl %es
		popl %ds
		popal
		addl $8, %esp
		iret

isrnoerror	0x00
isrnoerror	0x01
isrnoerror	0x02
isrnoerror	0x03
isrnoerror	0x04
isrnoerror	0x05
isrnoerror	0x06
isrnoerror	0x07
isrerror	0x08
isrnoerror	0x09
isrerror	0x0a
isrerror	0x0b
isrerror	0x0c
isrerror	0x0d
isrerror	0x0e
isrnoerror	0x0f
isrerror	0x10
isrnoerror	0x11
isrnoerror	0x12
isrnoerror	0x13
isrnoerror	0x14
isrnoerror	0x15
isrnoerror	0x16
isrnoerror	0x17
isrnoerror	0x18
isrnoerror	0x19
isrnoerror	0x1a
isrnoerror	0x1b
isrnoerror	0x1c
isrnoerror	0x1d
isrnoerror	0x1e
isrnoerror	0x1f
isrnoerror	0x20
isrnoerror	0x21
