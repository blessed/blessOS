#ifndef PATA_H
#define PATA_H

#include <kernel/common.h>

#define ATA_BASE 0x1f0
#define ATA_PRI_DATA	(ATA_BASE + 0)
#define ATA_PRI_ERR		(ATA_BASE + 1)
#define ATA_PRI_SECT	(ATA_BASE + 2)
#define ATA_PRI_SECTN	(ATA_BASE + 3)
#define ATA_PRI_CYLL	(ATA_BASE + 4)
#define ATA_PRI_CYLH	(ATA_BASE + 5)
#define ATA_PRI_HEAD	(ATA_BASE + 6)
#define ATA_PRI_CMD		(ATA_BASE + 7)

#define ATA_HD_CMD_PORT	0x3f6

#define ATA_CMD_READ	0x20
#define ATA_CMD_WRITE	0x30

#define ATA_SEEK_BIT	0x10
#define ATA_RDY_BIT		0x40
#define ATA_BUSY_BIT	0x80
#define ATA_DRQ_BIT		0x08

#define ATA_MASTER		0xa0

inline void write_to_port(const char *buf, int port, int count);

void initialize_pata(void);

void hd_rw(u32int drive, u32int nsect, u32int sect_no, u32int head,
		u32int cyl, u32int cmd);

void reset_hd_controller(void);

int is_hd_busy(void);
int is_hd_drq(void);

#endif /* PATA_H */
