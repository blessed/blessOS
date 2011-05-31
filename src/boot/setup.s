.code32
.text
.globl startup_32, gdt, idt, pg_dir
.extern main
.org 0

# paging directory is going to start from 0x0 and is going to overwrite following code
pg_dir:
startup_32:
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	
	call setup_gdt
	call setup_idt

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%fs
	movw %ax,%gs
	movw %ax,%ss
	movl $0xa0000,%esp

	movl $msg,%esi
	movl $0xb8000,%edi
	movl $10,%ecx
repeat:
	movb (%esi),%al
	movb %al,(%edi)
	addl $1,%edi
	movb $0x07,%al
	movb %al,(%edi)
	addl $1,%esi
	addl $1,%edi
	decw %cx
	jnz repeat

	xorl %eax,%eax
1:	incl %eax 		# test the A20 line, if it's enabled
	movl %eax,0x000000
	cmpl %eax,0x100000
	je 1b
	movl %cr0,%eax # looking for the math chip
	andl $0x80000011,%eax
	testl $0x10,%eax
	jne 1f		# ET is set - 387 is present
	orl $4,%eax	# else set emulate bit
1:	movl %eax,%cr0
	jmp start_main

boom:
	nop
	jmp boom

.align 2
setup_idt:
	lea ignore_int,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8E00,%dx

	lea idt,%edi
	movl $256,%ecx
repeat_load_idt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	decl %ecx
	jne repeat_load_idt
	lidt idt_descr
	ret

setup_gdt:
	lgdt gdt_descr
	ret

.org 0x1000
pg0:

.org 0x2000
pg1:

.org 0x3000
pg2:

.org 0x4000
start_main:
	sti
	pushl $0
	pushl $0
	pushl $0
	pushl $boom
	pushl $main
	jmp setup_paging

.align 2
setup_paging:
	movl $1024*3,%ecx
	xorl %eax,%eax
	xorl %edi,%edi
	cld;rep;stosl
	movl $pg0+7,pg_dir
	movl $pg1+7,pg_dir+4
	movl $pg1+4092,%edi
	movl $0x7ff007,%eax
	std
1:	stosl
	subl $0x1000,%eax
	jge 1b
	xorl %eax,%eax
	movl %eax,%cr3
	movl %cr0,%eax
	orl $0x80000000,%eax
	movl %eax,%cr0
	call main

ignore_int:
	incb 0xb8000+160
	movb $2,0xb8000+161
	iret

idt_descr:
	.word 256*8-1
	.long idt

idt:
	.fill 256, 8, 0

gdt_descr:
	.word 256*8-1
	.long gdt

gdt:
	.quad 0x00
	.quad 0x00C09A00000007FF
	.quad 0x00C09200000007FF
	.quad 0x00
	.fill 252,8,0

msg:
	.ascii "Hell Yeah!"
