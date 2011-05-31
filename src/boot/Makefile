AS = as
ASFLAGS = --32
AS16 = nasm
AS16FLAGS = -fbin

%.o : %.s
	$(AS) $(ASFLAGS) $< -o $@

boot: boot.s
	$(AS16) $(AS16FLAGS) boot.s 
