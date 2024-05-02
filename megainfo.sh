#!/bin/sh
set -e
test -e /sys/kernel/debug/megaraid_sas
grep -o -P '^\d\d\d(?= megaraid_sas_ioctl)' /proc/devices > /dev/null

[[ -e /dev/megaraid_sas_ioctl_node ]] || mknod /dev/megaraid_sas_ioctl_node c `grep -o -P '^\d\d\d(?= megaraid_sas_ioctl)' /proc/devices` 0
P=$(readlink -f $1)
host_no=$(echo "${P}" | /usr/bin/grep -o -P '(?<=host)\d+')

test -e /sys/kernel/debug/megaraid_sas/scsi_host${host_no}/raidmap_dump
exec /usr/lib/udev/megainfo $host_no /dev/megaraid_sas_ioctl_node $2
