#! /bin/sh

UBOOT_NAME=wr703n_tuboot_test_2012_03_28.bin
UBOOT_IMAGE_URL=192.168.1.2


wget http://$UBOOT_IMAGE_URL/$UBOOT_NAME

RAW_UBOOT_LEN=`wc -c $UBOOT_NAME | awk '{print $1 }'`
NEED_PAD_LEN=$((0x1fc00-$RAW_UBOOT_LEN))

#Generate a file used as pad ...
dd if=/dev/zero of=pad.bin bs=1 count=$NEED_PAD_LEN

cat $UBOOT_NAME pad.bin >tuboot_0x1fc00.bin

echo "Backup some config first,just like MAC address ..."
dd if=/dev/mtd0 of=./config.bin bs=1 skip=$((0x1fc00))

cat ./tuboot_0x1fc00.bin ./config.bin >uboot_0x20000.bin

cat uboot_0x20000.bin >/dev/mtdblock0

sync

