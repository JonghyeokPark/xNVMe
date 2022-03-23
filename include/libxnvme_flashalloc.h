/**
 * utilities to Falshalloc specific tasks on top of the Cosmos+ Open SSD
 * 
 *
 * @file xnvme_flashalloc.h
 */

#ifndef __XNVME_FLASHALLOC_H
#define __XNVME_FLASHALLOC_H 

#include <linux/types.h>
#include <stdint.h>
#include <libxnvme_dev.h>

#define IO_NVM_WRITE 					0x01
#define IO_NVM_READ 					0x02
#define IO_NVM_PRINT_STAT 		0x80
#define IO_NVM_RESET 					0x81
#define IO_NVM_FLASHALLOC 		0x82
#define IO_NVM_SET_LEVEL 			0x83
#define IO_NVM_SET_FILE 			0x84
#define MAGIC_NUM 						2621440

struct nvme_passthru_cmd {
  __u8 opcode;
  __u8 flags;
  __u16 rsvd1;
  __u32 nsid;
  __u32 cdw2;
  __u32 cdw3;
  __u64 metadata;
  __u64 addr;
  __u32 metadata_len;
  __u32 data_len;
  __u32 cdw10;
  __u32 cdw11;
  __u32 cdw12;
  __u32 cdw13;
  __u32 cdw14;
  __u32 cdw15;
  __u32 timeout_ms;
  __u32 result;
};

int
submit_ioctl(int fd, int opcode, __u32 data_len, __u32 lba, char**result);

int 
xnvme_flashalloc(struct xnvme_dev *dev, __u32 lba, __u32 len);

#endif
