INCLUDE_DIR = $(KERNEL_SRC)/include

AS = as
ASFLAGS = --32
CC = gcc
CFLAGS = -Wall -Wno-main -fomit-frame-pointer -m32 -pedantic -nostdinc -nostdlib -fno-builtin -I$(INCLUDE_DIR)
LDFLAGS = --oformat binary -mi386linux -Tscript.ld

.PHONY: all clean

KERNEL_SRC = /home/blessed/Programowanie/blessOS/original/src

OBJECTS = boot/setup.o init/print.o kernel/printk.o kernel/console.o \
			 kernel/sched.o kernel/mem.o kernel/exceptions.o kernel/isr.o \
			 kernel/keyboard.o kernel/timer.o kernel/paging.o kernel/kheap.o

%.o : %.s
	@$(AS) $(ASFLAGS) $< -o $@

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@

# kernel config
setup: boot/setup.o
	@$(LD) $(LDFLAGS) $< -o $@

# kernel binary
main: $(OBJECTS)
	@$(LD) $(LDFLAGS) $^ -o $@ -M > main.map

all: main image
	@echo -n "Writing to floppy... "
	@echo "Done"

image:
	dd of=floppy.img if=main bs=1 seek=512

clean:
	@echo -n "Cleaning... "
	@rm -f $(KERNEL_SRC)/kernel/*.o
	@rm -f $(KERNEL_SRC)/*.o
	@echo "Done"

