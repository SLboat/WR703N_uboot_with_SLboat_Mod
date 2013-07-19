#! /bin/sh


#high chance need have a change ...
UBOOT_NAME=wr703n_tuboot_test_2012_06_06.bin
MD5SUM_SHOULD_BE="623dc0bba6fab68c22e5fb2f329d7d09"
UBOOT_IMAGE_URL=192.168.1.2



wget http://$UBOOT_IMAGE_URL/$UBOOT_NAME

#need check the md5sum,any one byte in bootloader shoud right ...
CURRENT_MD5SUM_VAL=$( md5sum $UBOOT_NAME |awk '{print $1 }' )
echo "$UBOOT_NAME md5sum : $CURRENT_MD5SUM_VAL"

if [ $MD5SUM_SHOULD_BE = $CURRENT_MD5SUM_VAL ]; then
	echo "$UBOOT_NAME md5sum check pass"
else
	echo "###############$UBOOT_NAME md5sum check fail###############"
	exit
fi

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

