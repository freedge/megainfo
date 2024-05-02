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

#include "megaraid_sas.h"
#include "mfi.h"


enum {
	DATA_LEN = 512
};

static void autoclose(const void* ptr) {
	const int* fd = (const int*) ptr;
	if (*fd > 0)
		close(*fd);
}

// only dump the name if it contains a specific set of chars
bool is_ok(const char* p) {
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

// read the wwn from inquiry page 0x83
bool read_wwn(char* buf, const uint8_t* page83) {
	// reading the doc https://support.hpe.com/hpesc/public/docDisplay?docId=sd00001714en_us&docLocale=en_US&page=GUID-D7147C7F-2016-0901-065E-000000000484.html
	// however page_length is on byte 3 in practice
	if ((page83[0] == 0)          && // supported peripheral
	    (page83[3] == 20)         && // page length
	    ((page83[4] & 0x03) == 1) && // codeset: 1 binary
	    ((page83[5] & 0x03) == 3) && // identifier type: NAA
    	    (page83[7] == 16)         && // identifier length
	    ((page83[8] & 0xF0) == 0x60) // NAA 6 format
	   ){
		for (int i = 8; i < 24; i++) {
			buf += sprintf(buf, "%02x", page83[i]);
		}
		return true;

	}
	return false;
}

int main(const int argc, const char* argv[]) {
	if (argc != 4 && argc != 5) {
		fprintf(stderr, "usage: %s host_no ld_no /dev/megaraid_sas_ioctl_node [wwn]\n\n"
				"This also retrieves the inquiry page that can be read with\n"
				"  sg_inq --inhex=<(echo $MEGA_LD_VPD_PAGE83 | tr _ ' ')\n"
				"Count of ld available for each host_no can be taken from\n"
				"  od /sys/kernel/debug/megaraid_sas/scsi_host0/raidmap_dump -j 32 -N 2 -i -A none\n"
				"If wwn is specified, only dump information if this wwn is matching\n", argv[0]);
		return -1;
	}
	if (argc == 5 && 32 != strlen(argv[4])) {
		return -1;
	}
	const int fd __attribute__ ((__cleanup__(autoclose))) = open(argv[3], O_RDONLY);
	if (fd < 1) {
		fprintf(stderr, "open failed %s . Note that device needs to be created with\n"
		     "  mknod /dev/megaraid_sas_ioctl_node c `grep -o -P '^\\d\\d\\d(?= megaraid_sas_ioctl)' /proc/devices` 0\n", strerror(errno));
		return -1;
	}

	char ld_info_buf[DATA_LEN];
	memset(ld_info_buf, '\0', sizeof(ld_info_buf));
	struct mfi_ld_info* ld_info = (struct mfi_ld_info*) &ld_info_buf;

	struct megasas_iocpacket ioc = {
		.host_no = atoi(argv[1]),
		.sgl_off = 40,
		.sge_count = 1,
		.sgl = {
			{
				.iov_len = DATA_LEN,
				.iov_base = ld_info
			}
		},
		.frame.dcmd = {
			.cmd = MFI_CMD_DCMD,
			.cmd_status = 0,
			.opcode = MFI_DCMD_LD_GET_INFO,
			.data_xfer_len = DATA_LEN,
			.flags = MFI_FRAME_DIR_READ,
			.sge_count = 1,
			.sgl.sge32 = {
				.length = DATA_LEN,
				//.phys_addr = (__le32)(uintptr_t) ld_info
				.phys_addr = 0
			},
			.mbox.w = {
				atoi(argv[2])
			}
		}
	};

	// the actual work:
	const int res = ioctl (fd, MEGASAS_IOC_FIRMWARE, &ioc);

	if (res < 0) {
		fprintf(stderr, "ioctl failed %s\n", strerror(errno));
		return -1;
	}
	if (ioc.frame.dcmd.cmd_status != MFI_STAT_OK) {
		fprintf(stderr, "command failed 0x%02x%s\n", ioc.frame.dcmd.cmd_status,
				ioc.frame.dcmd.cmd_status == MFI_STAT_DEVICE_NOT_FOUND ? " (device not found)" : "");
		return -1;
	}
	if (ld_info->vpd_page83[1] != 0x83) {
		fprintf(stderr, "invalid page code 0x%02x\n", ld_info->vpd_page83[1]);
		return -1;
	}
	if (ld_info->ld_config.params.span_depth > MFI_MAX_SPAN_DEPTH) {
		fprintf(stderr, "invalid span depth\n");
	}
	char wwn[33];
	memset(wwn, '\0', sizeof(wwn));
	if (read_wwn(wwn, ld_info->vpd_page83)) {
		if (argc == 5 && strncmp(wwn, argv[4], sizeof(wwn)) != 0) {
			return 1;
		}
		printf("MEGA_LD_WWN=%s\n", wwn);
	} else {
		if (argc == 5) {
			return 1;
		}
	}


	const char* buf = ld_info->ld_config.properties.name;
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
	for (uint8_t i = 0; i < ld_info->vpd_page83[3] + 8; i++) {
		printf("%02x_", ld_info->vpd_page83[i]);
	}
	printf("\n");
	printf("MEGA_LD_CO_RA=%d,%d\n", ld_info->cluster_owner, ld_info->reconstruct_active);

	return 0;
}
