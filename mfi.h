/*
 * NetBSD header file, copied from
 * http://gitorious.org/freebsd/freebsd/blobs/HEAD/sys/dev/mfi/mfireg.h
 */
/*-
 * Copyright (c) 2006 IronPort Systems
 * Copyright (c) 2007 LSI Corp.
 * Copyright (c) 2007 Rajesh Prabhakaran.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#pragma once
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

