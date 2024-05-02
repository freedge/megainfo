#include <sys/ioctl.h>
#include <pci/types.h>
#include <linux/uio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <locale.h>

#include "megaraid_sas.h"
#include "mfi.h"
#include "megaraid_sas_fusion.h"


enum {
	DATA_LEN = 512
};

static void autoclose(const int* fd) {
	if (*fd > 0)
		close(*fd);
}

static void autofreelocale(const locale_t* l) {
	if (*l)
		freelocale(*l);
}

static bool sanitize_name(char* p) {
	if (*p == 0 || *p == ' ') {
		return false;
	}
	locale_t clocale __attribute__ ((__cleanup__(autofreelocale))) = newlocale(LC_ALL_MASK, "C", 0);
	assert(clocale);

	for (int i = 0; *p && i < 16; i++, p++) {
		if (isalnum_l(*p, clocale) ||
		    (*p == '-') ||
		    (*p == '_')) {
			// is fine
		} else if (*p == ' ') {
			*p = '_';
		} else {
			fprintf(stderr, "found invalid character in name 0x%02x\n", *p);
			return false;
		}
	}
	return true;
}

// read the wwn from inquiry page 0x83
static bool read_wwn(char* buf, const uint8_t* page83) {
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

// read the raidmap from the megaraid_sas debug file
static int parse_raid_map(struct MR_DRV_RAID_MAP* raid_map, int hostno) {
	char path[256];
	assert(sprintf(path, "/sys/kernel/debug/megaraid_sas/scsi_host%d/raidmap_dump", hostno) > 0);
	const int fd __attribute__ ((__cleanup__(autoclose))) = open(path, O_RDONLY);
	if (fd < 1) {
		fprintf(stderr, "%s open failed %s\n", path, strerror(errno));
		return -1;
	}
	assert(read(fd, raid_map, sizeof(*raid_map)) == sizeof(*raid_map));

	return raid_map->ldCount;
}

int main(const int argc, const char* argv[]) {
	if (argc != 3 && argc != 4) {
		fprintf(stderr, "usage: %s host_no /dev/megaraid_sas_ioctl_node [wwn]\n\n"
				"This also retrieves the inquiry page that can be read with\n"
				"  sg_inq --inhex=<(echo $MEGA_LD_VPD_PAGE83 | tr _ ' ')\n"
				"If wwn is specified, only dump information if this wwn is matching\n", argv[0]);
		return -1;
	}
	if (argc == 4 && 32 != strlen(argv[3])) {
		return -1;
	}

	int hostno = atoi(argv[1]);
	const int fd __attribute__ ((__cleanup__(autoclose))) = open(argv[2], O_RDONLY);
	if (fd < 1) {
		fprintf(stderr, "%s open failed %s . Note that device needs to be created with\n"
		     "  mknod /dev/megaraid_sas_ioctl_node c `grep -o -P '^\\d\\d\\d(?= megaraid_sas_ioctl)' /proc/devices` 0\n", argv[2], strerror(errno));
		return -1;
	}

	struct MR_DRV_RAID_MAP raid_map;
	int ldCount = parse_raid_map(&raid_map, hostno);
	if (ldCount <= 0) {
		return ldCount;
	}
	
	// we are looking for a maximum of ldCount device, however if devices were created an deleted, there
	// could be some gaps, we rely on the extracted raid map to tell us which id to use.
	for (int i = 0; ldCount > 0 && i < MAX_RAIDMAP_LOGICAL_DRIVES; i++) {
		if (raid_map.ldTgtIdToLd[i] == 255)
			continue;
		ldCount -= 1;

		printf("# host%d vd%d ld%d\n", hostno, i, raid_map.ldTgtIdToLd[i]);
		char ld_info_buf[DATA_LEN];
		memset(ld_info_buf, '\0', sizeof(ld_info_buf));
		struct mfi_ld_info* ld_info = (struct mfi_ld_info*) &ld_info_buf;

		struct megasas_iocpacket ioc = {
			.host_no = hostno,
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
					i
				}
			}
		};

		// the actual work:
		const int res = ioctl (fd, MEGASAS_IOC_FIRMWARE, &ioc);

		if (res < 0) {
			fprintf(stderr, "ioctl failed %s\n", strerror(errno));
			return -1;
		}
		if (ioc.frame.dcmd.cmd_status == MFI_STAT_DEVICE_NOT_FOUND) {
			fprintf(stderr, "device %d not found\n", i);
			return -1;
		}
		if (ioc.frame.dcmd.cmd_status != MFI_STAT_OK) {
			fprintf(stderr, "command failed on device %d, 0x%02x\n", i, ioc.frame.dcmd.cmd_status);
			return -1;
		}
		if (ld_info->vpd_page83[1] != 0x83) {
			fprintf(stderr, "invalid page code on device %d, 0x%02x\n", i, ld_info->vpd_page83[1]);
			return -1;
		}
		if (ld_info->ld_config.params.span_depth > MFI_MAX_SPAN_DEPTH) {
			fprintf(stderr, "invalid span depth on device %d\n", i);
		}
		char wwn[33];
		memset(wwn, '\0', sizeof(wwn));
		if (read_wwn(wwn, ld_info->vpd_page83)) {
			if (argc == 4 && strncmp(wwn, argv[3], sizeof(wwn)) != 0) {
				continue;
			}
			printf("MEGA_LD_WWN=%s\n", wwn);
		} else {
				continue;
		}


		char* buf = ld_info->ld_config.properties.name;
		if (sanitize_name(buf)) {
			printf("MEGA_LD_NAME=%.16s\n", buf);
		}
	}
	return 0;
}
