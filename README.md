[![Docker Repository on Quay](https://quay.io/repository/frigault/megainfo/status "Docker Repository on Quay")](https://quay.io/repository/frigault/megainfo)
[![Create and publish a Docker image](https://github.com/freedge/megainfo/actions/workflows/ci.yaml/badge.svg)](https://github.com/freedge/megainfo/actions/workflows/ci.yaml)

Find the virtual disk name / wwn association.

```
# for a given host find the amount of available vd. Both numbers will need to be specified
od /sys/kernel/debug/megaraid_sas/scsi_host0/raidmap_dump -j 32 -N 2 -i -A none
# create the character device
mknod /dev/megaraid_sas_ioctl_node c `grep -o -P '^\d\d\d(?= megaraid_sas_ioctl)' /proc/devices` 0

megainfo 0 0 /dev/megaraid_sas_ioctl_node
# or
podman run --network=none --cap-add sys_admin --device /dev/megaraid_sas_ioctl_node quay.io/frigault/megainfo:latest 0 0 /dev/megaraid_sas_ioctl_node
# or
podman run --network=none --cap-add sys_admin --device /dev/megaraid_sas_ioctl_node ghcr.io/freedge/megainfo:main 0 1 /dev/megaraid_sas_ioctl_node

MEGA_LD_NAME=vd-root
MEGA_LD_PROPERTIES=5,0,0,5,0
MEGA_LD_PARAMS=1,0,0,7,2,1,3,0,1
MEGA_LD_SIZE=1561591808
MEGA_LD_VPD_PAGE83=00_83_00_14_...
MEGA_LD_WWN=6d0946...
```

To just dump all the vd names and their wwn:
```
for i in `seq 0 $(($(od /sys/kernel/debug/megaraid_sas/scsi_host0/raidmap_dump -j 32 -N 2 -i -A none )- 1))` ;do bash -c "source <(./megainfo 0 $i  /dev/megaraid_sas_ioctl_node) && [[ -n \${MEGA_LD_NAME} ]] && echo \${MEGA_LD_NAME}=\${MEGA_LD_WWN}"; done
```

To plug that into udev: see megainfo.sh and 69-mine.rule


References:
- https://github.com/bonzini/qemu/blob/master/hw/scsi/megasas.c
- https://github.com/hmage/megactl/blob/master/megaioctl.c
- https://github.com/torvalds/linux/blob/master/drivers/scsi/megaraid/megaraid_sas.h
- https://github.com/smartmontools/smartmontools/blob/master/smartmontools/os_linux.cpp
- https://github.com/hreinecke/sg3_utils/blob/master/scripts/55-scsi-sg3_id.rules
- https://github.com/torvalds/linux/blob/5eb4573ea63d0c83bf58fb7c243fc2c2b6966c02/drivers/scsi/megaraid/megaraid_sas_fusion.h#L1179

