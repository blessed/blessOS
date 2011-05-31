[bits 16]

BOOTSEG equ 0x07c0
INITSEG equ 0x9000
SYSSEG  equ 0x1000
SYSSIZE equ 0x7000

; first let's move the boot code out of the way, so it won't
; be overwritten by the kernel code that is loaded at 0x1000
move_boot:
	mov ax,BOOTSEG
	mov ds,ax
	mov ax,INITSEG
	mov es,ax
	xor si,si
	xor di,di
	mov cx,256
	rep
	movsw

	jmp INITSEG:start ; code is copied, jump to it

start:
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov sp,0x400 ; stack is 512KB away from the data
	; print a message to the screen
	mov si,msg
	call putstr

	mov ax,SYSSEG
	mov es,ax
	call load_kernel ; loads kernel at 0x10000
	call stop_motor

	; store the cursor position somewhere
	mov ah,0x03
	xor bh,bh
	int 0x10
	mov[510],dx ; store it in 0x90510

	cli

; Moves the system to 0x0. This overwrites the standard idt
	mov ax,0x0000
	cld ; 'direction'=0, movs moves forward
move_kernel:
	mov es,ax
	add ax,0x1000
	cmp ax,0x9000
	jz end_kernel_move
	mov ds,ax
	xor si,si
	xor di,di
	mov cx,0x8000
	rep
	movsw
	jmp move_kernel

end_kernel_move:
	mov ax,cs
	mov ds,ax

	lgdt [gdtr]
	lidt [idtr]
	
	; reprogram the 8259
	mov al, 0x11 ; initialization sequence
	out 0x20, al ; send it to master
	dw 0x00eb, 0x00eb
	out 0xA0, al ; send it to slave
	dw 0x00eb, 0x00eb
	mov al, 0x20 ; start of hardware interrupts (#1)
	out 0x21, al
	dw 0x00eb, 0x00eb
	mov al, 0x28 ; start of hardware interrupts (#2)
	out 0xa1, al
	dw 0x00eb, 0x00eb
	mov al, 0x04 ; 8259-1 is master
	out 0x21, al
	dw 0x00eb, 0x00eb
	mov al, 0x02 ; 8259-2 is slave
	out 0xa1, al
	dw 0x00eb, 0x00eb
	mov al, 0x01 ; 8086 mode for both
	out 0x21, al
	dw 0x00eb, 0x00eb
	out 0xa1, al
	dw 0x00eb, 0x00eb
	mov al, 0xff ; mask off all interrupts for now
	out 0x21, al
	dw 0x00eb, 0x00eb
	out 0xa1, al

	mov eax,cr0
	or al,1
	mov cr0,eax

	jmp 0x0008:do_pm

hang:
	jmp $

; writes a character on screen
; si = address of message to be printed
; NOTE: message must end with ASCII character 0!
putstr:
	lodsb ; AL = DS:SI
	or al,al
	jz .done
	mov ah,0x0e
	mov bx,01110000b
	int 0x10
	jmp putstr

.done:
	ret

; load the kernel into memory at 0x10000
; ES - segment to which we should write the data
; BX - index in that segment
; sectors - already loaded sectors of current track (initially 1 because of the bootcode)
; head - currently used head
; track - currently used track

ENDSEG equ SYSSEG + SYSSIZE
TOTALSECTORS equ 18

sectors dw 1
head dw 0
track dw 0

load_kernel:
	mov ax, es
	test ax, 0x0fff	; check if it's on segment boundary (64Kb)

.die:
	jne .die

	xor bx, bx
.repeat_read:		; test if the full kernel is loaded
	mov ax, es
	cmp ax, ENDSEG
	jb .calc_sectors	; if not start loading but first calc sectors
	ret	; otherwise we can return

.calc_sectors:
	mov ax, TOTALSECTORS
	sub ax, [sectors]
	mov cx, ax
	shl cx, 9	; calc number of bytes to be read
	add cx, bx
	jnc .read_track
	je .read_track
	xor ax, ax
	sub ax, bx
	shr ax, 9

.read_track:
	call read_sectors
	mov cx, ax
	add ax, [sectors]
	cmp ax, TOTALSECTORS
	jne .update
	mov ax, 1
	sub ax, [head]
	jne .change_head
	inc word [track]

.change_head:
	mov [head], ax
	xor ax, ax

.update:
	mov [sectors], ax
	shl cx, 9
	add bx, cx
	jnc .repeat_read
	mov ax, es
	add ax, 0x1000
	mov es, ax
	xor bx, bx
	jmp .repeat_read

read_sectors:
	push ax
	push bx
	push cx
	push dx
	mov dx, [track]
	mov cx, [sectors]
	inc cx
	mov ch, dl
	mov dx, [head]
	mov dh, dl
	mov dl, 0
	and dx, 0x0100
	mov ah, 2
	int 0x13
	jc .bad_rt
	pop dx
	pop cx
	pop bx
	pop ax
	ret

.bad_rt:
	mov ax, 0
	mov dx, 0
	int 0x13
	pop dx
	pop cx
	pop bx
	pop ax
	jmp read_sectors

stop_motor:
	push dx
	mov dx, 0x3f2
	mov al, 0
	out dx, al
	pop dx
	ret

[BITS 32]
do_pm:
	mov ax,SYS_DATA_SEL
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov fs,ax
	mov gs,ax

	mov esp, 0xa500

	jmp 0x08:0x00

[BITS 16]

gdtr:
	dw gdt_end - gdt - 1
	dw gdt,0x9 ; the gdt table is at segment 0x90000

gdt:
	times 8 db 0	; dummy used by Intel only

SYS_CODE_SEL equ $ - gdt
	dw 0x07FF
	dw 0
	db 0
	db 0x9A
	db 0xC0
	db 0

SYS_DATA_SEL equ $ - gdt
	dw 0x07FF
	dw 0
	db 0
	db 0x92
	db 0xC0
	db 0

SYS_CODE_SEL_2 equ $ - gdt
	dw 0x07ff
	dw 0x0000
	db 9
	db 0x9a
	db 0xc0
	db 0

SYS_DATA_SEL_2 equ $ - gdt
	dw 0x07ff
	dw 0x0000
	db 9
	db 0x92
	db 0xc0
	db 0

gdt_end:

idtr:
	dw idt_end - idt - 1
	dw idt, 0x9 ; the idt table is at segment 0x90000

idt:
	db 0
idt_end:

msg db 13,10,'Loading System...',13,10,0

times 510-($-$$) db 0
dw 0xaa55 ; bootcode stamp
