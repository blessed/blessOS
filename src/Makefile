INCLUDE_DIR = $(KERNEL_SRC)/include

AS = as
ASFLAGS = --32
CC = gcc
CFLAGS = -Wall -fomit-frame-pointer -m32 -pedantic -nostdinc -nostdlib -fno-builtin -I$(INCLUDE_DIR)
LDFLAGS = --oformat binary -mi386linux -Tscript.ld

KERNEL_SRC = /home/blessed/Programowanie/blessOS/original/src

%.o : %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# kernel config
setup: boot/setup.o
	$(LD) $(LDFLAGS) $< -o $@

# kernel binary
main: boot/setup.o kernel/print.o
	$(LD) $(LDFLAGS) $^ -o $@

all: main
	dd of=floppy.img if=$< bs=1 seek=512

clean:
	rm -f $(KERNEL_SRC)/kernel/*.o
	rm -f $(KERNEL_SRC)/*.o


