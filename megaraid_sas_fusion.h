/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Linux MegaRAID driver for SAS based RAID controllers
 *
 *  Copyright (c) 2009-2013  LSI Corporation
 *  Copyright (c) 2013-2016  Avago Technologies
 *  Copyright (c) 2016-2018  Broadcom Inc.
 *
 *  FILE: megaraid_sas_fusion.h
 *
 *  Authors: Broadcom Inc.
 *           Manoj Jose
 *           Sumant Patro
 *           Kashyap Desai <kashyap.desai@broadcom.com>
 *           Sumit Saxena <sumit.saxena@broadcom.com>
 *
 *  Send feedback to: megaraidlinux.pdl@broadcom.com
 */
#pragma once

struct MR_DRV_RAID_MAP {
	/* total size of this structure, including this field.
	 * This feild will be manupulated by driver for ext raid map,
	 * else pick the value from firmware raid map.
	 */
	__le32                 totalSize;

	union {
	struct {
		__le32         maxLd;
		__le32         maxSpanDepth;
		__le32         maxRowSize;
		__le32         maxPdCount;
		__le32         maxArrays;
	} validationInfo;
	__le32             version[5];
	};

	/* timeout value used by driver in FP IOs*/
	u8                  fpPdIoTimeoutSec;
	u8                  reserved2[7];

	__le16                 ldCount;
	__le16                 arCount;
	__le16                 spanCount;
	__le16                 reserve3;


};

struct MR_DRV_RAID_MAP_ALL {

	struct MR_DRV_RAID_MAP raidMap;
} __packed;
