
COMPRESSED_TOOLCHAIN_PACKAGE_NAME=OpenWrt-Toolchain-ar71xx-for-mips_r2-gcc-4.6-linaro_uClibc-0.9.33.tar.bz2
DECOMPRESS_TOOLCHAIN_DIRECTORY_NAME=OpenWrt-Toolchain-ar71xx-for-mips_r2-gcc-4.6-linaro_uClibc-0.9.33

all:
	@if [ ! -e $(DECOMPRESS_TOOLCHAIN_DIRECTORY_NAME) ];then \
	echo "Decompress toolchain for your,please wait ...\n";\
	tar -jxvf $(COMPRESSED_TOOLCHAIN_PACKAGE_NAME);\
	fi

clean:
	echo "########## Remove the compressed toolchain for you ##########"
	rm -rf $(DECOMPRESS_TOOLCHAIN_DIRECTORY_NAME)

