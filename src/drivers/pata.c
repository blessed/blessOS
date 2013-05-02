#include <asm/io.h>
#include <kernel/printk.h>
#include <drivers/pata.h>

inline void write_to_port(const char *buf, int port, int count)
{
	asm("cld;\n\t"
			"rep;\n\t"
			"outsw\n\t"
			::"d"(port), "S"(buf), "c"(count));
}

inline void read_from_port(char *buf, int port, int count)
{
	asm("cld;\n\t"
			"rep;\n\t"
			"insw\n\t"
			::"d"(port), "D"(buf), "c"(count));
}

void initialize_pata(void)
{
	outb(inb(0x21) & ~0x04, 0x21);
	outb(inb(0xa1) & ~0x40, 0xa1);
}

static int is_hd_ready(void)
{
	int retries = 10000;

	while (retries--)
	{
		if ((inb(ATA_PRI_CMD) & 0xc0) == ATA_RDY_BIT)
			break;
	}

	return retries;
}

void hd_rw(u32int drive, u32int nsect, u32int sect_no, u32int head,
		u32int cyl, u32int cmd)
{
	if (!is_hd_ready())
	{
		printk(KPL_PANIC, "HD controller doesn't respond\n");
		return;
	}

	printk(KPL_DUMP, "%s:%d: %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
	printk(KPL_DUMP, "%x\n", cmd);

	BOCHS_DEBUG();

	outb(ATA_MASTER | (drive << 4) | head, ATA_PRI_HEAD);
	printk(KPL_DUMP, "Sent %x to %x\n", ATA_MASTER | (drive << 4) | head, ATA_PRI_HEAD);
	outb(nsect, ATA_PRI_SECT);
	outb(sect_no, ATA_PRI_SECTN);
	outb(cyl & 0xff, ATA_PRI_CYLL);
	outb(cyl >> 8, ATA_PRI_CYLH);
	outb(cmd, ATA_PRI_CMD);
}

void reset_hd_controller(void)
{
	outb(4, ATA_HD_CMD_PORT);
	int i;
	for (i = 0; i < 10000; ++i)
		__asm__("nop");
	outb(0, ATA_HD_CMD_PORT);

	if (is_hd_busy())
		printk(KPL_DUMP, "HD controller is busy\n");
}

int is_hd_busy(void)
{
	int i;

	for (i = 0; i < 10000; ++i)
		if (ATA_RDY_BIT == (inb(ATA_PRI_CMD) & (ATA_BUSY_BIT | ATA_RDY_BIT)))
			break;

	i = inb(ATA_PRI_CMD);
	i &= ATA_BUSY_BIT | ATA_RDY_BIT | ATA_SEEK_BIT;
	if (i == (ATA_SEEK_BIT | ATA_RDY_BIT))
	{
		printk(KPL_DUMP, "HD controller is now ready\n");
		return 0;
	}

	printk(KPL_DUMP, "HD controller timed out\n");

	return 1;
}

int is_hd_drq(void)
{
	if (inb(ATA_PRI_CMD) & ATA_DRQ_BIT)
		return 1;

	return 0;
}
