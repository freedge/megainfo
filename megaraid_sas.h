/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Linux MegaRAID driver for SAS based RAID controllers
 *
 *  Copyright (c) 2003-2013  LSI Corporation
 *  Copyright (c) 2013-2016  Avago Technologies
 *  Copyright (c) 2016-2018  Broadcom Inc.
 *
 *  FILE: megaraid_sas.h
 *
 *  Authors: Broadcom Inc.
 *           Kashyap Desai <kashyap.desai@broadcom.com>
 *           Sumit Saxena <sumit.saxena@broadcom.com>
 *
 *  Send feedback to: megaraidlinux.pdl@broadcom.com
 */

#pragma once
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
		struct megasas_dcmd_frame dcmd; /* added for megainfo */
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



/*
 * MFI command completion codes
 */
enum MFI_STAT {
	MFI_STAT_OK = 0x00,
	MFI_STAT_INVALID_CMD = 0x01,
	MFI_STAT_INVALID_DCMD = 0x02,
	MFI_STAT_INVALID_PARAMETER = 0x03,
	MFI_STAT_INVALID_SEQUENCE_NUMBER = 0x04,
	MFI_STAT_ABORT_NOT_POSSIBLE = 0x05,
	MFI_STAT_APP_HOST_CODE_NOT_FOUND = 0x06,
	MFI_STAT_APP_IN_USE = 0x07,
	MFI_STAT_APP_NOT_INITIALIZED = 0x08,
	MFI_STAT_ARRAY_INDEX_INVALID = 0x09,
	MFI_STAT_ARRAY_ROW_NOT_EMPTY = 0x0a,
	MFI_STAT_CONFIG_RESOURCE_CONFLICT = 0x0b,
	MFI_STAT_DEVICE_NOT_FOUND = 0x0c,
	MFI_STAT_DRIVE_TOO_SMALL = 0x0d,
	MFI_STAT_FLASH_ALLOC_FAIL = 0x0e,
	MFI_STAT_FLASH_BUSY = 0x0f,
	MFI_STAT_FLASH_ERROR = 0x10,
	MFI_STAT_FLASH_IMAGE_BAD = 0x11,
	MFI_STAT_FLASH_IMAGE_INCOMPLETE = 0x12,
	MFI_STAT_FLASH_NOT_OPEN = 0x13,
	MFI_STAT_FLASH_NOT_STARTED = 0x14,
	MFI_STAT_FLUSH_FAILED = 0x15,
	MFI_STAT_HOST_CODE_NOT_FOUNT = 0x16,
	MFI_STAT_LD_CC_IN_PROGRESS = 0x17,
	MFI_STAT_LD_INIT_IN_PROGRESS = 0x18,
	MFI_STAT_LD_LBA_OUT_OF_RANGE = 0x19,
	MFI_STAT_LD_MAX_CONFIGURED = 0x1a,
	MFI_STAT_LD_NOT_OPTIMAL = 0x1b,
	MFI_STAT_LD_RBLD_IN_PROGRESS = 0x1c,
	MFI_STAT_LD_RECON_IN_PROGRESS = 0x1d,
	MFI_STAT_LD_WRONG_RAID_LEVEL = 0x1e,
	MFI_STAT_MAX_SPARES_EXCEEDED = 0x1f,
	MFI_STAT_MEMORY_NOT_AVAILABLE = 0x20,
	MFI_STAT_MFC_HW_ERROR = 0x21,
	MFI_STAT_NO_HW_PRESENT = 0x22,
	MFI_STAT_NOT_FOUND = 0x23,
	MFI_STAT_NOT_IN_ENCL = 0x24,
	MFI_STAT_PD_CLEAR_IN_PROGRESS = 0x25,
	MFI_STAT_PD_TYPE_WRONG = 0x26,
	MFI_STAT_PR_DISABLED = 0x27,
	MFI_STAT_ROW_INDEX_INVALID = 0x28,
	MFI_STAT_SAS_CONFIG_INVALID_ACTION = 0x29,
	MFI_STAT_SAS_CONFIG_INVALID_DATA = 0x2a,
	MFI_STAT_SAS_CONFIG_INVALID_PAGE = 0x2b,
	MFI_STAT_SAS_CONFIG_INVALID_TYPE = 0x2c,
	MFI_STAT_SCSI_DONE_WITH_ERROR = 0x2d,
	MFI_STAT_SCSI_IO_FAILED = 0x2e,
	MFI_STAT_SCSI_RESERVATION_CONFLICT = 0x2f,
	MFI_STAT_SHUTDOWN_FAILED = 0x30,
	MFI_STAT_TIME_NOT_SET = 0x31,
	MFI_STAT_WRONG_STATE = 0x32,
	MFI_STAT_LD_OFFLINE = 0x33,
	MFI_STAT_PEER_NOTIFICATION_REJECTED = 0x34,
	MFI_STAT_PEER_NOTIFICATION_FAILED = 0x35,
	MFI_STAT_RESERVATION_IN_PROGRESS = 0x36,
	MFI_STAT_I2C_ERRORS_DETECTED = 0x37,
	MFI_STAT_PCI_ERRORS_DETECTED = 0x38,
	MFI_STAT_CONFIG_SEQ_MISMATCH = 0x67,

	MFI_STAT_INVALID_STATUS = 0xFF
};

