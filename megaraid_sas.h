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

