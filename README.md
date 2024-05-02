[![Docker Repository on Quay](https://quay.io/repository/frigault/megainfo/status "Docker Repository on Quay")](https://quay.io/repository/frigault/megainfo)
[![Create and publish a Docker image](https://github.com/freedge/megainfo/actions/workflows/ci.yaml/badge.svg)](https://github.com/freedge/megainfo/actions/workflows/ci.yaml)

Find the virtual disk name / wwn association. Use at your own risks.

I made that to see how difficult it was to find out virtual disk names from Linux, without using a proprietary tool, since there is no way to get the wwn from the bmc for a virtual drive, but it's possible to set up a user friendly name, and access it from Linux. That should allow the creation of a device with udev.

PERC:

That one uses an ioctl on the megaraid_sas_ioctl character device.

```
# create the character device
mknod /dev/megaraid_sas_ioctl_node c `grep -o -P '^\d\d\d(?= megaraid_sas_ioctl)' /proc/devices` 0

# there should be a megaraid debug file for the host under /sys/kernel/debug/megaraid_sas/scsi_host0/raidmap_dump

megainfo 0 /dev/megaraid_sas_ioctl_node
# or
podman run --network=none --privileged --volume /sys/kernel/debug:/sys/kernel/debug --device /dev/megaraid_sas_ioctl_node quay.io/frigault/megainfo:latest 0 /dev/megaraid_sas_ioctl_node
# or
podman run --network=none --privileged --volume /sys/kernel/debug:/sys/kernel/debug --device /dev/megaraid_sas_ioctl_node ghcr.io/freedge/megainfo:main 0 /dev/megaraid_sas_ioctl_node

MEGA_LD_NAME=vd-root
MEGA_LD_WWN=6d0946...
```

To plug that into udev: see megainfo.sh and 69-megainfo.rules

Packaged as an RPM https://copr.fedorainfracloud.org/coprs/frigo/megainfo/

BOSS:

Also adding a rule for Marvell (Dell BOSS) controllers vd names.
That one uses an ioctl for SG_IO on the "Marvell Console" 88SE9230 device.

```
# udevadm info -q property --property=ID_SERIAL_SHORT /dev/sdd
ID_SERIAL_SHORT=ATA_DELLBOSS_VD_d4d4eb9afeaa0010

# bash bossinfo.sh ATA_DELLBOSS_VD_d4d4eb9afeaa0010
BOSS_LD_NAME=vd-boss
```

References:
- https://github.com/bonzini/qemu/blob/master/hw/scsi/megasas.c
- https://github.com/hmage/megactl/blob/master/megaioctl.c
- https://github.com/torvalds/linux/blob/master/drivers/scsi/megaraid/megaraid_sas.h
- https://github.com/smartmontools/smartmontools/blob/master/smartmontools/os_linux.cpp
- https://github.com/hreinecke/sg3_utils/blob/master/scripts/55-scsi-sg3_id.rules
- https://github.com/torvalds/linux/blob/5eb4573ea63d0c83bf58fb7c243fc2c2b6966c02/drivers/scsi/megaraid/megaraid_sas_fusion.h#L1179

