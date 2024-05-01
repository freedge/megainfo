#!/bin/sh

# $1: ATA_DELLBOSS_VD_d4d4eb9afeaa0010

source $(dirname `grep -l Marvell /sys/class/scsi_device/*/device/vendor`)/generic/uevent

/usr/bin/sg_senddiag /dev/${DEVNAME} --raw e0,00,01,84,04,00,38,00,f1,02,00,00,00,00,00,00,00,00,00,00,00,00,00,01,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,48,01,00,00,00,02,00,00,01,00,00,00,00,00,00,00,00,00,00,00,00,00,ff,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff,00,ff > /dev/null
res=$(/usr/bin/sg_ses /dev/${DEVNAME} -p 0xe1 -HHH | tr -d ' ' | sed -e 's/../\\x&/g')
name=$(echo -ne $res | od -j 114 -N 16  -A none -c | tr -d ' ' | grep -o -P '^[a-zA-Z0-9@_-]*')
serial=$(echo -ne $res | od -j 90 -N 8  -A none -t x1 | tr -d ' ' | sed -e 's/\(.\)\(.\)/\2\1/g')

[[ ${name} != '' ]] && [[ $1 == ATA_DELLBOSS_VD_${serial} ]] && echo BOSS_LD_NAME=${name}
