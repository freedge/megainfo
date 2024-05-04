#!/bin/sh
set -e

# $1: ATA_DELLBOSS_VD_d4d4eb9afeaa0010

source $(dirname `grep -l Marvell /sys/class/scsi_device/*/device/vendor`)/generic/uevent

SENDPAGE="e0,00,01,84"
_CDB="f1,02,00,00,00,00,00,00,00,00,00,00,00,00,00,01"
_SENSEBUFFER="00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00"
SENDPARAM="04,00,38,00,${_CDB},${_SENSEBUFFER},48,01,00,00"
_RESERVED1="00,00,00,00,00,00"
LDINFOREQ="00,02,00,00,01,00,00,00,00,00,${_RESERVED1},00"


/usr/bin/sg_senddiag /dev/${DEVNAME} --raw \
        ${SENDPAGE},${SENDPARAM},${LDINFOREQ},00,ff,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff > /dev/null

# struct _LD_Info {
#     u16;  # offset 76
#     u8;
#     u8;
#     u16;
#     u8;
#     u8;
#     u8;
#     u8 LD_GUID[8];
#     u8;
#     u8;
#     u8;
#     u16;
#     u16;
#     u64;
#     u8 Name[16];
#     u16;
#     u8;
#     u8;
#     u16;
#     u32;
# }
res=$(/usr/bin/sg_ses /dev/${DEVNAME} -p 0xe1 -HHH | tr -d ' \n' | sed -e 's/../\\x&/g')
name=$(echo -ne $res | od -j 108 -N 16  -A none -c | tr -d ' ' | grep -o -P '^[a-zA-Z0-9@_-]*')
serial=$(echo -ne $res | od -j 85 -N 8  -A none -t x1 | tr -d ' ' | sed -e 's/\(.\)\(.\)/\2\1/g')

[[ ${name} != '' ]] && [[ $1 == ATA_DELLBOSS_VD_${serial} ]] && echo BOSS_LD_NAME=${name}
