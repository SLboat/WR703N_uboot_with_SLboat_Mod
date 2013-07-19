/*
 * (C) Copyright 2000-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Boot support
 */
#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <zlib.h>
#include <bzlib.h>
#include <LzmaWrapper.h>
#include <environment.h>
#include <asm/byteorder.h>

DECLARE_GLOBAL_DATA_PTR;

/*cmd_boot.c*/
extern int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

#if (CONFIG_COMMANDS & CFG_CMD_DATE) || defined(CONFIG_TIMESTAMP)
#include <rtc.h>
#endif

#ifdef CONFIG_SHOW_BOOT_PROGRESS
#include <status_led.h>
#define SHOW_BOOT_PROGRESS(arg)	show_boot_progress(arg)
#else
#define SHOW_BOOT_PROGRESS(arg)
#endif

#ifdef CONFIG_LOGBUFFER
#include <logbuff.h>
#endif

/*
 * Some systems (for example LWMON) have very short watchdog periods;
 * we must make sure to split long operations like memmove() or
 * crc32() into reasonable chunks.
 */
#if defined(CONFIG_HW_WATCHDOG) || defined(CONFIG_WATCHDOG)
#define CHUNKSZ (64 * 1024)
#endif

int gunzip(void *, int, unsigned char *, unsigned long *);

#if (CONFIG_COMMANDS & CFG_CMD_IMI)
static int image_info (unsigned long addr);
#endif

#if (CONFIG_COMMANDS & CFG_CMD_IMLS)
#include <flash.h>
extern flash_info_t flash_info[]; /* info for FLASH chips */
static int do_imls (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif

static void print_type(image_header_t *hdr);

#ifdef __I386__
image_header_t *fake_header(image_header_t *hdr, void *ptr, int size);
#endif

/*
 *  Continue booting an OS image; caller already has:
 *  - copied image header to global variable `header'
 *  - checked header magic number, checksums (both header & image),
 *  - verified image architecture (PPC) and type (KERNEL or MULTI),
 *  - loaded (first part of) image to header load address,
 *  - disabled interrupts.
 */
typedef void boot_os_Fcn(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], ulong addr, ulong *len_ptr, int verify);

#ifdef DEBUG
extern int do_bdinfo( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif

extern boot_os_Fcn do_bootm_linux;

#ifdef CONFIG_SILENT_CONSOLE
static void fixup_silent_linux (void);
#endif

#if (CONFIG_COMMANDS & CFG_CMD_ELF)
static boot_os_Fcn do_bootm_vxworks;
static boot_os_Fcn do_bootm_qnxelf;
int do_bootvx ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] );
int do_bootelf (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] );
#endif /* CFG_CMD_ELF */

#ifndef CFG_BOOTM_LEN
#define CFG_BOOTM_LEN	0x800000	/* use 8MByte as default max gunzip size */
#endif

image_header_t header;
ulong load_addr = CFG_LOAD_ADDR; /* Default Load Address */

/* changed by lqm, 18Jan08 */
#include "tpLinuxTag.h"		/* support TP-LINK Linux Tag */

/* added by lqm, 18Jan08, copy from fake_zimage_header() */
image_header_t *fake_image_header(image_header_t *hdr, ulong kernelTextAddr, ulong entryPoint, int size) {
	ulong checksum = 0;

	memset(hdr, 0, sizeof(image_header_t));

	/* Build new header */
	hdr->ih_magic = htonl(IH_MAGIC);
	hdr->ih_time = 0;
	hdr->ih_size = htonl(size);
	hdr->ih_load = htonl(kernelTextAddr);
	hdr->ih_ep = htonl(entryPoint);
	hdr->ih_dcrc = htonl(checksum);
	hdr->ih_os = IH_OS_LINUX;
	hdr->ih_arch = IH_CPU_MIPS;
	hdr->ih_type = IH_TYPE_KERNEL;
	hdr->ih_comp = IH_COMP_GZIP;

	strncpy((char *) hdr->ih_name, "(none)", IH_NMLEN);

	hdr->ih_hcrc = htonl(checksum);

	return hdr;
}

int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]) {
	//ulong iflag;
	ulong addr;
	ulong data, len;
	ulong *len_ptr;
	uint unc_len = CFG_BOOTM_LEN;
	int i, verify = 0;
	image_header_t *hdr = &header;
	LINUX_FILE_TAG *fileTag;		// by lqm, 17Sep07

	if (argc < 2) {
		addr = load_addr;
	} else {
		addr = simple_strtoul(argv[1], NULL, 16);
	}

	//SHOW_BOOT_PROGRESS (1);

	printf("Booting image at: 0x%08lX...\n", addr);

	fileTag = (LINUX_FILE_TAG *) addr;
	/*
	printf("---- fileTag = %08x\n", fileTag);
	printf("\t text base = %08x\n", fileTag->kernelTextAddr);
	printf("\t entry point = %08x\n", fileTag->kernelEntryPoint);
	*/
	fake_image_header(hdr, fileTag->kernelTextAddr, fileTag->kernelEntryPoint, fileTag->kernelLen);

	data = addr + TAG_LEN;
	len = ntohl(hdr->ih_size);

	/* TODO:  */
	//SHOW_BOOT_PROGRESS (2);
	//SHOW_BOOT_PROGRESS (6);
	/*
	 * We have reached the point of no return: we are going to
	 * overwrite all exception vector code, so we cannot easily
	 * recover from any failures any more...
	 */
	//iflag = disable_interrupts();
	// TODO: disable_interrupts() only returns 0...
	//disable_interrupts();
#ifdef CONFIG_AR7100
	/*
	 * Flush everything, restore caches for linux
	 */
	mips_cache_flush();

	/* XXX - this causes problems when booting from flash */
	/* dcache_disable(); */
#endif

	/*	case IH_COMP_LZMA:*/
	puts("Uncompressing kernel image...\n");

	i = lzma_inflate((unsigned char *) data, len, (unsigned char*) ntohl(hdr->ih_load), (int *) &unc_len);

	if (i != LZMA_RESULT_OK) {
		//printf("## Error: LZMA error number %d\n", i);
		//SHOW_BOOT_PROGRESS(-6);
		//udelay(100000);
		return -1;
		//do_reset(cmdtp, flag, argc, argv);
	}

	puts("OK\n");
	//SHOW_BOOT_PROGRESS(7);

	/*	case IH_OS_LINUX: */
#ifdef CONFIG_SILENT_CONSOLE
	fixup_silent_linux();
#endif

	do_bootm_linux(cmdtp, flag, argc, argv, addr, len_ptr, verify);

	//SHOW_BOOT_PROGRESS(-9);
#ifdef DEBUG
	puts ("\n## Error: control returned to monitor - resetting...\n");
	do_reset (cmdtp, flag, argc, argv);
#endif
	return 1;
}

U_BOOT_CMD( bootm, CFG_MAXARGS, 1, do_bootm, "boot application image from memory\n", "[addr [arg ...]]\n"
"\t- boot application image stored in memory at address 'addr'\n"
"\t  passing arguments 'arg ...'; when booting a Linux kernel,\n"
"\t  'arg' can be the address of an initrd image\n");

#ifdef CONFIG_SILENT_CONSOLE
static void
fixup_silent_linux ()
{
	char buf[256], *start, *end;
	char *cmdline = getenv ("bootargs");

	/* Only fix cmdline when requested */
	if (!(gd->flags & GD_FLG_SILENT))
	return;

	debug ("before silent fix-up: %s\n", cmdline);
	if (cmdline) {
		if ((start = strstr (cmdline, "console=")) != NULL) {
			end = strchr (start, ' ');
			strncpy (buf, cmdline, (start - cmdline + 8));
			if (end)
			strcpy (buf + (start - cmdline + 8), end);
			else
			buf[start - cmdline + 8] = '\0';
		} else {
			strcpy (buf, cmdline);
			strcat (buf, " console=");
		}
	} else {
		strcpy (buf, "console=");
	}

	setenv ("bootargs", buf);
	debug ("after silent fix-up: %s\n", buf);
}
#endif /* CONFIG_SILENT_CONSOLE */

#if (CONFIG_COMMANDS & CFG_CMD_BOOTD)
int do_bootd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int rcode = 0;
	if (run_command (getenv ("bootcmd"), flag) < 0) rcode = 1;
	return rcode;
}

U_BOOT_CMD(boot, 1, 1, do_bootd, "boot default - run command 'bootcmd'\n", NULL);

/* keep old command name "bootd" for backward compatibility */
U_BOOT_CMD(bootd, 1, 1, do_bootd, "boot default, i.e., run 'bootcmd'\n", NULL);
#endif

#if (CONFIG_COMMANDS & CFG_CMD_IMI)
int do_iminfo ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int arg;
	ulong addr;
	int rcode=0;

	if (argc < 2) {
		return image_info (load_addr);
	}

	for (arg=1; arg <argc; ++arg) {
		addr = simple_strtoul(argv[arg], NULL, 16);
		if (image_info (addr) != 0) rcode = 1;
	}
	return rcode;
}

static int image_info (ulong addr)
{
	ulong data, len, checksum;
	image_header_t *hdr = &header;

	printf ("\n## Checking Image at %08lx ...\n", addr);

	/* Copy header so we can blank CRC field for re-calculation */
	memmove (&header, (char *)addr, sizeof(image_header_t));

	if (ntohl(hdr->ih_magic) != IH_MAGIC) {
		puts ("   Bad Magic Number\n");
		return 1;
	}

	data = (ulong)&header;
	len = sizeof(image_header_t);

	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;

	if (crc32 (0, (uchar *)data, len) != checksum) {
		puts ("   Bad Header Checksum\n");
		return 1;
	}

	/* for multi-file images we need the data part, too */
	print_image_hdr ((image_header_t *)addr);

	data = addr + sizeof(image_header_t);
	len = ntohl(hdr->ih_size);

	puts ("   Verifying Checksum ... ");
	if (crc32 (0, (uchar *)data, len) != ntohl(hdr->ih_dcrc)) {
		puts ("   Bad Data CRC\n");
		return 1;
	}
	puts ("OK\n");
	return 0;
}

U_BOOT_CMD(
		iminfo,
		CFG_MAXARGS,
		1,
		do_iminfo,
		"print header information for application image\n",
		"addr [addr ...]\n"
		"\t- print header information for application image starting at\n"
		"\t  address 'addr' in memory; this includes verification of the\n"
		"\t  image contents (magic number, header and payload checksums)\n"
);

#endif	/* CFG_CMD_IMI */

#if (CONFIG_COMMANDS & CFG_CMD_IMLS)
/*-----------------------------------------------------------------------
 * List all images found in flash.
 */
int do_imls (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	flash_info_t *info;
	int i, j;
	image_header_t *hdr;
	ulong data, len, checksum;

	for (i=0, info=&flash_info[0]; i<CFG_MAX_FLASH_BANKS; ++i, ++info) {
		if (info->flash_id == FLASH_UNKNOWN)
		goto next_bank;
		for (j=0; j<info->sector_count; ++j) {

			if (!(hdr=(image_header_t *)info->start[j]) ||
					(ntohl(hdr->ih_magic) != IH_MAGIC))
			goto next_sector;

			/* Copy header so we can blank CRC field for re-calculation */
			memmove (&header, (char *)hdr, sizeof(image_header_t));

			checksum = ntohl(header.ih_hcrc);
			header.ih_hcrc = 0;

			if (crc32 (0, (uchar *)&header, sizeof(image_header_t))
					!= checksum)
			goto next_sector;

			printf ("Image at %08lX:\n", (ulong)hdr);
			print_image_hdr( hdr );

			data = (ulong)hdr + sizeof(image_header_t);
			len = ntohl(hdr->ih_size);

			puts ("   Verifying Checksum ... ");
			if (crc32 (0, (uchar *)data, len) != ntohl(hdr->ih_dcrc)) {
				puts ("   Bad Data CRC\n");
			}
			puts ("OK\n");
			next_sector:;
		}
		next_bank:;
	}

	return (0);
}

U_BOOT_CMD(
		imls,
		1,
		1,
		do_imls,
		"list all images found in flash\n",
		"\n"
		"\t- prints information about all images found at sector boundaries in flash.\n"
);
#endif	/* CFG_CMD_IMLS */

void print_image_hdr(image_header_t *hdr) {
#if (CONFIG_COMMANDS & CFG_CMD_DATE) || defined(CONFIG_TIMESTAMP)
	time_t timestamp = (time_t)ntohl(hdr->ih_time);
	struct rtc_time tm;
#endif

	printf("   Image Name:   %.*s\n", IH_NMLEN, hdr->ih_name);
#if (CONFIG_COMMANDS & CFG_CMD_DATE) || defined(CONFIG_TIMESTAMP)
	to_tm (timestamp, &tm);
	printf ("   Created:      %4d-%02d-%02d  %2d:%02d:%02d UTC\n",
			tm.tm_year, tm.tm_mon, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);
#endif	/* CFG_CMD_DATE, CONFIG_TIMESTAMP */
	puts("   Image Type:   ");
	print_type(hdr);
	printf("\n   Data Size:    %d Bytes = ", ntohl(hdr->ih_size));
	print_size(ntohl(hdr->ih_size), "\n");
	printf("   Load Address: %08x\n"
			"   Entry Point:  %08x\n", ntohl(hdr->ih_load), ntohl(hdr->ih_ep));

	if (hdr->ih_type == IH_TYPE_MULTI) {
		int i;
		ulong len;
		ulong *len_ptr = (ulong *) ((ulong) hdr + sizeof(image_header_t));

		puts("   Contents:\n");
		for (i = 0; (len = ntohl(*len_ptr)); ++i, ++len_ptr) {
			printf("   Image %d: %8ld Bytes = ", i, len);
			print_size(len, "\n");
		}
	}
}

static void print_type(image_header_t *hdr) {
	char *os, *arch, *type, *comp;

	switch (hdr->ih_os) {
		case IH_OS_INVALID:
			os = "Invalid OS";
			break;
		case IH_OS_NETBSD:
			os = "NetBSD";
			break;
		case IH_OS_LINUX:
			os = "Linux";
			break;
		case IH_OS_VXWORKS:
			os = "VxWorks";
			break;
		case IH_OS_QNX:
			os = "QNX";
			break;
		case IH_OS_U_BOOT:
			os = "U-Boot";
			break;
		case IH_OS_RTEMS:
			os = "RTEMS";
			break;
		default:
			os = "Unknown OS";
			break;
	}

	switch (hdr->ih_arch) {
		case IH_CPU_INVALID:
			arch = "Invalid CPU";
			break;
		case IH_CPU_ALPHA:
			arch = "Alpha";
			break;
		case IH_CPU_ARM:
			arch = "ARM";
			break;
		case IH_CPU_I386:
			arch = "Intel x86";
			break;
		case IH_CPU_IA64:
			arch = "IA64";
			break;
		case IH_CPU_MIPS:
			arch = "MIPS";
			break;
		case IH_CPU_MIPS64:
			arch = "MIPS 64 Bit";
			break;
		case IH_CPU_PPC:
			arch = "PowerPC";
			break;
		case IH_CPU_S390:
			arch = "IBM S390";
			break;
		case IH_CPU_SH:
			arch = "SuperH";
			break;
		case IH_CPU_SPARC:
			arch = "SPARC";
			break;
		case IH_CPU_SPARC64:
			arch = "SPARC 64 Bit";
			break;
		case IH_CPU_M68K:
			arch = "M68K";
			break;
		case IH_CPU_MICROBLAZE:
			arch = "Microblaze";
			break;
		case IH_CPU_NIOS:
			arch = "Nios";
			break;
		case IH_CPU_NIOS2:
			arch = "Nios-II";
			break;
		default:
			arch = "Unknown Architecture";
			break;
	}

	switch (hdr->ih_type) {
		case IH_TYPE_INVALID:
			type = "Invalid Image";
			break;
		case IH_TYPE_STANDALONE:
			type = "Standalone Program";
			break;
		case IH_TYPE_KERNEL:
			type = "Kernel Image";
			break;
		case IH_TYPE_RAMDISK:
			type = "RAMDisk Image";
			break;
		case IH_TYPE_MULTI:
			type = "Multi-File Image";
			break;
		case IH_TYPE_FIRMWARE:
			type = "Firmware";
			break;
		case IH_TYPE_SCRIPT:
			type = "Script";
			break;
		default:
			type = "Unknown Image";
			break;
	}

	switch (hdr->ih_comp) {
		case IH_COMP_NONE:
			comp = "uncompressed";
			break;
		case IH_COMP_GZIP:
			comp = "gzip compressed";
			break;
		case IH_COMP_BZIP2:
			comp = "bzip2 compressed";
			break;
		case IH_COMP_LZMA:
			comp = "lzma compressed";
			break;
		default:
			comp = "unknown compression";
			break;
	}

	printf("%s %s %s (%s)", arch, os, type, comp);
}

#define HEAD_CRC	2
#define EXTRA_FIELD	4
#define ORIG_NAME	8
#define COMMENT		0x10
#define RESERVED	0xe0

#if (CONFIG_COMMANDS & CFG_CMD_ELF)
static void
do_bootm_vxworks (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		ulong addr, ulong *len_ptr, int verify)
{
	image_header_t *hdr = &header;
	char str[80];

	sprintf(str, "%x", ntohl(hdr->ih_ep)); /* write entry-point into string */
	setenv("loadaddr", str);
	do_bootvx(cmdtp, 0, 0, NULL);
}

static void
do_bootm_qnxelf (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		ulong addr, ulong *len_ptr, int verify)
{
	image_header_t *hdr = &header;
	char *local_args[2];
	char str[16];

	sprintf(str, "%x", ntohl(hdr->ih_ep)); /* write entry-point into string */
	local_args[0] = argv[0];
	local_args[1] = str; /* and provide it via the arguments */
	do_bootelf(cmdtp, 0, 2, local_args);
}
#endif /* CFG_CMD_ELF */
