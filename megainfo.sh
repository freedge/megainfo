#!/bin/sh
set -e
grep -o -P '^\d\d\d(?= megaraid_sas_ioctl)' /proc/devices > /dev/null
[[ -e /dev/megaraid_sas_ioctl_node ]] || mknod /dev/megaraid_sas_ioctl_node c `grep -o -P '^\d\d\d(?= megaraid_sas_ioctl)' /proc/devices` 0
P=$(readlink -f $1)
host_no=$(echo "${P}" | /usr/bin/grep -o -P '(?<=host)\d+')
wwid=$(cat ${1}/wwid)

if [[ -e /sys/kernel/debug/megaraid_sas/scsi_host${host_no}/raidmap_dump ]] ; then
  for i in `seq 0 $(($(od /sys/kernel/debug/megaraid_sas/scsi_host*/raidmap_dump -j 32 -N 2 -i -A none )- 1))` ;do
    unset MEGA_LD_NAME
    unset MEGA_LD_WWN
    source <(/root/megainfo $host_no $i /dev/megaraid_sas_ioctl_node)
    if [[ $wwid = naa.${MEGA_LD_WWN} ]] ; then
        echo MEGA_LD_NAME=${MEGA_LD_NAME}
        exit 0
    fi
  done
fi
exit 1