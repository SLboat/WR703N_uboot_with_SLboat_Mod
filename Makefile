
export BUILD_TOPDIR=$(PWD)
MAKECMD=make ARCH=mips CROSS_COMPILE=mips-openwrt-linux-uclibc-
export PATH:=$(BUILD_TOPDIR)/toolchain/OpenWrt-Toolchain-ar71xx-for-mips_r2-gcc-4.6-linaro_uClibc-0.9.33/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33/bin/:$(PATH)
export STAGING_DIR=$(BUILD_TOPDIR)/tmp
export FLASH_SIZE=4
export COMPRESSED_UBOOT=1
export MAX_UBOOT_SIZE=130048 #size 0x1fc00
export CONFIG_HORNET_1_1_WAR=1
export NEW_DDR_TAP_CAL=1

#not export any PRODUCTOR_CPU_*,will run 400_400_200.
#export PRODUCTOR_CPU_350=1
#export PRODUCTOR_CPU_300=1

all: decompress_toolchain uboot
	@echo tuboot.bin size: `wc -c < $(BUILD_TOPDIR)/bin/tuboot.bin`
	@if [ "`wc -c < $(BUILD_TOPDIR)/bin/tuboot.bin`" -gt "$(MAX_UBOOT_SIZE)" ]; then \
			echo "####################ERROR####################" \
            echo "tuboot.bin image size more than $(MAX_UBOOT_SIZE)"; \
    fi;


decompress_toolchain:
	make -C $(BUILD_TOPDIR)/toolchain/


uboot:
	cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) tl-wr703n_config
	cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) ENDIANNESS=-EB V=1 all
	cp $(BUILD_TOPDIR)/u-boot/tuboot.bin $(BUILD_TOPDIR)/bin
	$(BUILD_TOPDIR)/u-boot/tools/mkhttpdimage -d $(BUILD_TOPDIR)/bin/tuboot.bin $(BUILD_TOPDIR)/bin/tlr.bin
	cat $(BUILD_TOPDIR)/bin/tuboot.bin $(BUILD_TOPDIR)/bin/tlr.bin >$(BUILD_TOPDIR)/bin/wr703n_uboot_with_tlr.bin

clean:
	cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) clean
	rm -f $(BUILD_TOPDIR)/bin/*
	

clean_all:
	cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) distclean
	rm -f $(BUILD_TOPDIR)/bin/*
	make -C $(BUILD_TOPDIR)/toolchain/ clean

