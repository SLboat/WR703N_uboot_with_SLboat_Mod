export BUILD_TOPDIR=$(PWD)
export MAKECMD=make --silent ARCH=mips CROSS_COMPILE=mips-openwrt-linux-uclibc-
export PATH:=$(BUILD_TOPDIR)/toolchain/bin/:$(PATH)
export STAGING_DIR=$(BUILD_TOPDIR)/tmp

# maximum U-Boot image size
export AP121_MAX_UBOOT_SIZE=65536

# boot delay (time to autostart boot command)
export AP121_CONFIG_BOOTDELAY=1

mr3020:	export UBOOT_FILE_NAME=uboot_for_mr3020
mr3020:
	@cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) mr3020_config
	@cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) ENDIANNESS=-EB V=1 all
	@cp $(BUILD_TOPDIR)/u-boot/tuboot.bin $(BUILD_TOPDIR)/bin/temp.bin
	@make show_size
	
wr703n:	export UBOOT_FILE_NAME=uboot_for_wr703n
wr703n:
	@cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) wr703n_config
	@cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) ENDIANNESS=-EB V=1 all
	@cp $(BUILD_TOPDIR)/u-boot/tuboot.bin $(BUILD_TOPDIR)/bin/temp.bin
	@make show_size
	
mr3040:	export UBOOT_FILE_NAME=uboot_for_mr3040
mr3040:
	@cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) mr3040_config
	@cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) ENDIANNESS=-EB V=1 all
	@cp $(BUILD_TOPDIR)/u-boot/tuboot.bin $(BUILD_TOPDIR)/bin/temp.bin
	@make show_size

wr740n_v4:	export UBOOT_FILE_NAME=uboot_for_wr740n_v4
wr740n_v4:
	@cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) wr740n_v4_config
	@cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) ENDIANNESS=-EB V=1 all
	@cp $(BUILD_TOPDIR)/u-boot/tuboot.bin $(BUILD_TOPDIR)/bin/temp.bin
	@make show_size

show_size:
	@echo "\n======= Preparing 64 KB file filled with 0xFF... ======="
	@`tr "\000" "\377" < /dev/zero | dd ibs=1k count=64 of=$(BUILD_TOPDIR)/bin/$(UBOOT_FILE_NAME).bin`
	@echo "\n======= Copying U-Boot image... ======="
	@`dd if=$(BUILD_TOPDIR)/bin/temp.bin of=$(BUILD_TOPDIR)/bin/$(UBOOT_FILE_NAME).bin conv=notrunc`
	@`rm $(BUILD_TOPDIR)/bin/temp.bin`
	@echo "\n======= U-Boot image ready, size:" `wc -c < $(BUILD_TOPDIR)/bin/$(UBOOT_FILE_NAME).bin`" bytes ======="
	@`md5sum $(BUILD_TOPDIR)/bin/$(UBOOT_FILE_NAME).bin | awk '{print $$1}' | tr -d '\n' > $(BUILD_TOPDIR)/bin/$(UBOOT_FILE_NAME).md5`
	@`echo ' *'$(UBOOT_FILE_NAME).bin >> $(BUILD_TOPDIR)/bin/$(UBOOT_FILE_NAME).md5`
	@if [ "`wc -c < $(BUILD_TOPDIR)/bin/$(UBOOT_FILE_NAME).bin`" -gt "$(AP121_MAX_UBOOT_SIZE)" ]; then \
			echo "\n**********************************"; \
            echo "*   U-BOOT IMAGE SIZE TOO BIG!   *"; \
            echo "**********************************"; \
    fi;

clean:
	cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) clean
	rm -f $(BUILD_TOPDIR)/bin/*

clean_all:
	cd $(BUILD_TOPDIR)/u-boot/ && $(MAKECMD) distclean
	rm -f $(BUILD_TOPDIR)/bin/*