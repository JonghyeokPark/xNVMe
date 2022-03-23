// xNvme Flashallocate Wrapper 
// (jhpark): current xnvme_flashalloc version is just **wrapper** of the vnailla FlashAlloc
// this function exists for 표준을 위해서 존재함.

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <libxnvme_flashalloc.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <xnvme_be_linux.h>

#define NVME_IOCTL_IO_CMD _IOWR('N', 0x43, struct nvme_passthru_cmd)

#ifdef XNVME_FLASHALLOC_DEBUG
void show_command(int opcode, void **buf, __u32 data_len, __u32 lba,
                  int isQuery) {
  printf("opcode       : %02x\n", opcode);
  printf("flags        : %02x\n", 0); 
  printf("rsvd1        : %04x\n", 0); 
  printf("nsid         : %08x\n", 0); 
  printf("cdw2         : %08x\n", 0); 
  printf("cdw3         : %08x\n", 0); 
  printf("data_len     : %08x\n", data_len);
  printf("metadata_len : %08x\n", 0); 
  printf("addr         : %" PRIx64 "\n", (uint64_t)(uintptr_t)buf);
  printf("metadata     : %08x\n", 0); 
  printf("cdw10        : %08x\n", lba);
  printf("cdw11        : %08x\n", 0); 
  printf("cdw12        : %08x\n", 0); 
  printf("cdw13        : %08x\n", (isQuery));
  printf("cdw14        : %08x\n", 0); 
  printf("cdw15        : %08x\n", 0); 
  printf("timeout_ms   : %08x\n", 0); 
}
#endif

/**
 * Submit ioctl command, this also  provides Cosmos+ OpenSSD specific fucntion
 * TODO(jhpark): do we need to trim auxiliary functions (e.g., SET_LEVEL, SET_FILE)?
 * 
 * @param opcode operation code
 * @param data_len
 * @param lba
 * @output result 
 * @return On success, return 0 otherwise, return -1
 */
int
submit_ioctl(int fd, int opcode, __u32 data_len, __u32 lba, char**result)
{ 
  int err;
  void *ioctl_buf = NULL;

  switch (opcode) {
    case IO_NVM_SET_LEVEL:
    case IO_NVM_SET_FILE:
      break;
    case IO_NVM_PRINT_STAT:
    case IO_NVM_RESET:
      data_len = 0;
      break;
    case IO_NVM_FLASHALLOC:
      break;
    default:
      break;
  }

  err = posix_memalign(&ioctl_buf, getpagesize(), data_len);
	if (err != 0) {
		// debug print
	}
#ifdef XNVME_FLASHALLOC_DEBUG
  show_command(opcode, ioctl_buf, data_len, lba, 0);
#endif

  struct nvme_passthru_cmd cmd = {
      .opcode = (uint8_t)opcode,
      .flags = 0,
      .rsvd1 = 0,
      .nsid = 0,
      .cdw2 = 0,
      .cdw3 = 0,
      .metadata = 0,
      .addr = (__u64)(uintptr_t)ioctl_buf,
      .metadata_len = 0,
      .data_len = 0,
      .cdw10 = lba,
      .cdw11 = 0,
      .cdw12 = 0,
      .cdw13 = 0,
      .cdw14 = data_len,
      .cdw15 = 0,
      .timeout_ms = 0,
      .result = 0,
  };

	err = ioctl(fd, NVME_IOCTL_IO_CMD, &cmd);

	if (ioctl_buf) {
		if (err == 0 && opcode == 2 && result != NULL) {
			*result = (char *)ioctl_buf;
		}
		free(ioctl_buf);
	}

	return err;
}

// TODO(jhpark): support various env
/*
static int
xnvme_file_opts_to_xxx(struct xnvme_opts *opts)
{
  int flags = 0;

  flags |= opts->create ? O_CREAT : 0x0;
  flags |= opts->direct ? O_DIRECT : 0x0;
  flags |= opts->rdonly ? O_RDONLY : 0x0;
  flags |= opts->wronly ? O_WRONLY : 0x0;
  flags |= opts->rdwr ? O_RDWR : 0x0;
  flags |= opts->truncate ? O_TRUNC : 0x0;

  return flags;
}
*/

/**
 * call FlashAlloc command using given start lba and length
 * 
 * @param lba start LBA of the allocation (e.g., 
 * @param len length of the allocation 
 * @return On sucess return 0, otherwise return -1
 */
int
xnvme_flashalloc(struct xnvme_dev *dev, __u32 lba, __u32 len) 
{
  int err;

//	const struct xnvme_ident *ident = &dev->ident;
//	struct xnvme_opts *opts = &dev->opts;
//	int flags = xnvme_file_opts_to_xxx(opts);
//	fd = open(ident->uri, flags, opts->create_mode);
  struct xnvme_be_linux_state *state = (void *)dev->be.state;
  int fd = state->fd;
  err = submit_ioctl(fd, IO_NVM_FLASHALLOC, len, lba, 0);
  return err;
}
