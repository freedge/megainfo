#include <sys/ioctl.h>
#include <pci/types.h>
#include <linux/uio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>


// bunch of stuff from Linux
#define MEGASAS_IOC_FIRMWARE	_IOWR('M', 1, struct megasas_iocpacket)
#define MAX_IOCTL_SGE			16


struct megasas_sge32 {

	__le32 phys_addr;
	__le32 length;

} __attribute__ ((packed));


#define DECLARE_FLEX_ARRAY(TYPE, NAME) \
	__DECLARE_FLEX_ARRAY(TYPE, NAME)

union megasas_sgl {
	struct megasas_sge32 sge32;
} __attribute__ ((packed));

struct megasas_dcmd_frame {

	u8 cmd;			/*00h */
	u8 reserved_0;		/*01h */
	u8 cmd_status;		/*02h */
	u8 reserved_1[4];	/*03h */
	u8 sge_count;		/*07h */

	__le32 context;		/*08h */
	__le32 pad_0;		/*0Ch */

	__le16 flags;		/*10h */
	__le16 timeout;		/*12h */

	__le32 data_xfer_len;	/*14h */
	__le32 opcode;		/*18h */

	union {			/*1Ch */
		u8 b[12];
		__le16 s[6];
		__le32 w[3];
	} mbox;

	union megasas_sgl sgl;	/*28h */

} __attribute__ ((packed));

struct megasas_header {

	u8 cmd;			/*00h */
	u8 sense_len;		/*01h */
	u8 cmd_status;		/*02h */
	u8 scsi_status;		/*03h */

	u8 target_id;		/*04h */
	u8 lun;			/*05h */
	u8 cdb_len;		/*06h */
	u8 sge_count;		/*07h */

	__le32 context;		/*08h */
	__le32 pad_0;		/*0Ch */

	__le16 flags;		/*10h */
	__le16 timeout;		/*12h */
	__le32 data_xferlen;	/*14h */

} __attribute__ ((packed));

struct megasas_iocpacket {

	u16 host_no;
	u16 __pad1;
	u32 sgl_off;
	u32 sge_count;
	u32 sense_off;
	u32 sense_len;
	union {
		u8 raw[128];
		struct megasas_header hdr;
	} frame;

	struct iovec sgl[MAX_IOCTL_SGE];

} __attribute__ ((packed));

enum MFI_CMD_OP {
	MFI_CMD_INIT		= 0x0,
	MFI_CMD_LD_READ		= 0x1,
	MFI_CMD_LD_WRITE	= 0x2,
	MFI_CMD_LD_SCSI_IO	= 0x3,
	MFI_CMD_PD_SCSI_IO	= 0x4,
	MFI_CMD_DCMD		= 0x5,
	MFI_CMD_ABORT		= 0x6,
	MFI_CMD_SMP		= 0x7,
	MFI_CMD_STP		= 0x8,
	MFI_CMD_NVME		= 0x9,
	MFI_CMD_TOOLBOX		= 0xa,
	MFI_CMD_OP_COUNT,
	MFI_CMD_INVALID		= 0xff
};

#define MFI_FRAME_SENSE64			0x0004
#define MFI_FRAME_DIR_NONE			0x0000
#define MFI_FRAME_DIR_WRITE			0x0008
#define MFI_FRAME_DIR_READ			0x0010
#define MFI_FRAME_IEEE                          0x0020
#define MFI_CMD_STATUS_POLL_MODE		0xFF

// extra stuff from qemu
enum {
     MFI_DCMD_LD_GET_LIST =              0x03010000,
     MFI_DCMD_LD_LIST_QUERY =            0x03010100,
     MFI_DCMD_LD_GET_INFO =              0x03020000
};

# define QEMU_PACKED __attribute__((packed))
#define MFI_MAX_SPAN_DEPTH      8
union mfi_ld_ref {
    struct {
        uint8_t target_id;
        uint8_t reserved;
        uint16_t seq;
    } v;
    uint32_t ref;
} QEMU_PACKED;

struct mfi_ld_props {
    union mfi_ld_ref ld;
    char name[16];
    uint8_t default_cache_policy;
    uint8_t access_policy;
    uint8_t disk_cache_policy;
    uint8_t current_cache_policy;
    uint8_t no_bgi;
    uint8_t reserved[7];
} QEMU_PACKED;

struct mfi_ld_params {
    uint8_t primary_raid_level;
    uint8_t raid_level_qualifier;
    uint8_t secondary_raid_level;
    uint8_t stripe_size;
    uint8_t num_drives;
    uint8_t span_depth;
    uint8_t state;
    uint8_t init_state;
    uint8_t is_consistent;
    uint8_t reserved[23];
} QEMU_PACKED;

struct mfi_span {
    uint64_t start_block;
    uint64_t num_blocks;
    uint16_t array_ref;
    uint8_t reserved[6];
} QEMU_PACKED;

struct mfi_ld_config {
    struct mfi_ld_props properties;
    struct mfi_ld_params params;
    struct mfi_span span[MFI_MAX_SPAN_DEPTH];
} QEMU_PACKED;

struct mfi_progress {
    uint16_t progress;
    uint16_t elapsed_seconds;
} QEMU_PACKED;

struct mfi_ld_progress {
    uint32_t            active;
#define MFI_LD_PROGRESS_CC      (1<<0)
#define MFI_LD_PROGRESS_BGI     (1<<1)
#define MFI_LD_PROGRESS_FGI     (1<<2)
#define MFI_LD_PORGRESS_RECON   (1<<3)
    struct mfi_progress cc;
    struct mfi_progress bgi;
    struct mfi_progress fgi;
    struct mfi_progress recon;
    struct mfi_progress reserved[4];
} QEMU_PACKED;

struct mfi_ld_info {
    struct mfi_ld_config ld_config;
    uint64_t size;
    struct mfi_ld_progress progress;
    uint16_t cluster_owner;
    uint8_t reconstruct_active;
    uint8_t reserved1[1];
    uint8_t vpd_page83[64];
    uint8_t reserved2[16];
} QEMU_PACKED;


// my own stuff then

enum {
	DATA_LEN = 512
};

void autofree(void* ptr_ptr) {
	void* ptr = (void*) *((uintptr_t*) ptr_ptr);
	free(ptr);
}
void automunmap(void* ptr_ptr) {
	void* ptr = (void*) *((uintptr_t*) ptr_ptr);
	assert(munmap(ptr, DATA_LEN) == 0);
}
void autoclose(void* ptr) {
	int* fd = (int*) ptr;
	if (*fd > 0)
		close(*fd);
}

bool is_ok(char* p) {
	for (int i = 0; *p && i < 16; i++, p++) {
		if ((*p >= '@' && *p <= 'Z') ||
		    (*p >= 'a' && *p <= 'z') ||
		    (*p >= '0' && *p <= '9') ||
		    (*p == '-') ||
		    (*p == '_')) {
			// is fine
		} else {
			fprintf(stderr, "found invalid character in name 0x%02x\n", *p);
			return false;
		}
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s host_no /dev/megaraid_sas_ioctl_node\n\n"
				"This also retrieves the inquiry page that can be read with\n"
				" sg_inq --inhex=<(echo $MEGA_LD_VPD_PAGE83 | tr _ ' ')\n", argv[0]);
		return -1;
	}
	int fd __attribute__ ((__cleanup__(autoclose))) = open(argv[2], O_RDONLY);
	if (fd < 1) {
		fprintf(stderr, "open failed %s . Note that device needs to be created with\n"
		     "  mknod /dev/megaraid_sas_ioctl_node c `grep -o -P '^\\d\\d\\d(?= megaraid_sas_ioctl)' /proc/devices` 0\n", strerror(errno));
		return -1;
	}


	struct megasas_iocpacket *ioc __attribute__ ((__cleanup__(autofree))) = malloc(sizeof(struct megasas_iocpacket));
	assert(ioc);
	memset(ioc, '\0', sizeof(struct megasas_iocpacket));
	ioc->host_no = atoi(argv[1]);
	ioc->sgl_off = 40;
	ioc->sge_count = 1;

	struct megasas_dcmd_frame *frame = (struct megasas_dcmd_frame *) &(ioc->frame.raw[0]);
	frame->cmd = MFI_CMD_DCMD;
	frame->cmd_status = 0;
	frame->opcode = MFI_DCMD_LD_GET_INFO;
	frame->data_xfer_len = DATA_LEN;
	frame->flags = MFI_FRAME_DIR_READ;
	frame->sge_count = 1;

	u8* u __attribute__ ((__cleanup__(automunmap))) = mmap(0, DATA_LEN,
    		PROT_READ | PROT_WRITE,
    		MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT,
    		-1, 0
	);
	assert(u != MAP_FAILED);
	memset(u, '\0', DATA_LEN);
	frame->sgl.sge32.phys_addr = (__le32)(uintptr_t) u;
	frame->sgl.sge32.length = DATA_LEN;
	ioc->sgl[0].iov_base = u;
	ioc->sgl[0].iov_len = DATA_LEN;

	int res = ioctl (fd, MEGASAS_IOC_FIRMWARE, ioc);
	if (res < 0) {
		fprintf(stderr, "ioctl failed %s\n", strerror(errno));
		return -1;
	}
	struct mfi_ld_info* ld_info = (struct mfi_ld_info*) u;
	if (ld_info->vpd_page83[1] != 0x83) {
		fprintf(stderr, "invalid page code\n");
		return -1;
	}
	if (ld_info->ld_config.params.span_depth > MFI_MAX_SPAN_DEPTH) {
		fprintf(stderr, "invalid span depth\n");
	}


	char* buf = ld_info->ld_config.properties.name;
	if (buf[0] && is_ok(buf)) {
		printf("MEGA_LD_NAME=%.16s\n", buf);
	}
	printf("MEGA_LD_PROPERTIES=%d,%d,%d,%d,%d,%d,%d\n", 
			ld_info->ld_config.properties.ld.v.target_id,
			ld_info->ld_config.properties.ld.v.seq,
			ld_info->ld_config.properties.default_cache_policy,
			ld_info->ld_config.properties.access_policy,
			ld_info->ld_config.properties.disk_cache_policy,
			ld_info->ld_config.properties.current_cache_policy,
			ld_info->ld_config.properties.no_bgi
	      ); 
	printf("MEGA_LD_PARAMS=%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
			ld_info->ld_config.params.primary_raid_level,
			ld_info->ld_config.params.raid_level_qualifier,
			ld_info->ld_config.params.secondary_raid_level,
			ld_info->ld_config.params.stripe_size,
			ld_info->ld_config.params.num_drives,
			ld_info->ld_config.params.span_depth,
			ld_info->ld_config.params.state,
			ld_info->ld_config.params.init_state,
			ld_info->ld_config.params.is_consistent
	      ); 
	for (int i = 0; i < ld_info->ld_config.params.span_depth; i++) {
		printf("MEGA_LD_SPAN_%d=%ld,%ld,%d", i,
				ld_info->ld_config.span[i].start_block,
				ld_info->ld_config.span[i].num_blocks,
				ld_info->ld_config.span[i].array_ref);
	}
	printf("\n");

	printf("MEGA_LD_SIZE=%ld\n", ld_info->size);
	printf("MEGA_LD_VPD_PAGE83=");
	for (int i = 0; i < 0x28; i++) {
		printf("%02x_", ld_info->vpd_page83[i]);
	}
	printf("\n");
	printf("MEGA_LD_WWN=");
	for (int i = 8; i < 24; i++) {
		printf("%02x", ld_info->vpd_page83[i]);
	}
	printf("\n");
	printf("MEGA_LD_CO_RA=%d,%d\n", ld_info->cluster_owner, ld_info->reconstruct_active);

	return 0;
}
