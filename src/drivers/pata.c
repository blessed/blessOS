#include <asm/io.h>
#include <kernel/printk.h>
#include <drivers/pata.h>
#include <kernel/mem.h>
#include <mm/kheap.h>

disk_info_t *main_disk_info;

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

void pata_sleep400(void)
{
	/* Each I/O port access can be assumed to be about 100nsec.
	 * PATA standard requires a 400ns delay after each command */
	inb(ATA_PRI_ASTAT);
	inb(ATA_PRI_ASTAT);
	inb(ATA_PRI_ASTAT);
	inb(ATA_PRI_ASTAT);
}

int pata_wait(u8int drive)
{
	u8int status;
	int i;

	for (i = 0; ; i++)
	{
		status = inb(ATA_PRI_CMD);

		if (!(status & ATA_BUSY_BIT))
			return 0;

		if (i > 4 && ((status && ATA_ERR_BIT) || (status && ATA_FAULT_BIT)))
			return status;
	}
}

void pata_select(u8int drive)
{
	outb(drive & 1 ? 0xb0 : 0xa0, ATA_PRI_HEAD);
	pata_sleep400();
}

void initialize_pata(void)
{
	printk(KPL_DUMP, "Initializing PATA\n");
	/* Unmask the hard disk interrupt with the PIC */
	outb(inb(0x21) & ~0x04, 0x21);
	outb(inb(0xa1) & ~0x40, 0xa1);

	//main_disk_info = identify_drive();

	//printk(KPL_DUMP, "Detected hard drive:\n");
	//printk(KPL_DUMP, "Cylinders %d\n", main_disk_info->geometry.cylinders);
	//printk(KPL_DUMP, "Heads %d\n", main_disk_info->geometry.heads);
	//printk(KPL_DUMP, "Sectors %d\n", main_disk_info->geometry.sectors);
	//printk(KPL_DUMP, "Sector size %d\n", main_disk_info->geometry.sector_size);
	//printk(KPL_DUMP, "Sectors per track %d\n", main_disk_info->geometry.sects_per_track);
	//BOCHS_DEBUG();
}

disk_info_t* identify_drive(void)
{
	printk(KPL_DUMP, "Identifying drives\n");

	u16int var;
	disk_info_t *tmp = (disk_info_t *)kmalloc(sizeof(disk_info_t));
	char *buf = (char *)kmalloc(256);
	memset(buf, 0, sizeof(256));

	return tmp;
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
