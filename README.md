Find the virtual disk name / wwn association.

```
mknod /dev/megaraid_sas_ioctl_node c `grep -o -P '^\d\d\d(?= megaraid_sas_ioctl)' /proc/devices` 0
megainfo 0 /dev/megaraid_sas_ioctl_node

MEGA_LD_NAME=vd-root
MEGA_LD_PROPERTIES=5,0,0,5,0
MEGA_LD_PARAMS=1,0,0,7,2,1,3,0,1
MEGA_LD_SIZE=1561591808
MEGA_LD_VPD_PAGE83=00_83_00_14_...
MEGA_LD_WWN=6d0946...
```

TODO:
- support more than /c0/v0
- write a udev rule to create device automatically


reference:
- https://github.com/bonzini/qemu/blob/master/hw/scsi/megasas.c
- https://github.com/hmage/megactl/blob/master/megaioctl.c
- https://github.com/torvalds/linux/blob/master/drivers/scsi/megaraid/megaraid_sas.h
- https://github.com/smartmontools/smartmontools/blob/master/smartmontools/os_linux.cpp
- https://github.com/hreinecke/sg3_utils/blob/master/scripts/55-scsi-sg3_id.rules

