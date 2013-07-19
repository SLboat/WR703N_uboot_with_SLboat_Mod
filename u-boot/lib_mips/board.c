/*
 * (C) Copyright 2003
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

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <devices.h>
#include <version.h>
#include <net.h>
#include <environment.h>

DECLARE_GLOBAL_DATA_PTR;

#if ( ((CFG_ENV_ADDR+CFG_ENV_SIZE) < CFG_MONITOR_BASE) || (CFG_ENV_ADDR >= (CFG_MONITOR_BASE + CFG_MONITOR_LEN)) ) || defined(CFG_ENV_IS_IN_NVRAM)
#define	TOTAL_MALLOC_LEN	(CFG_MALLOC_LEN + CFG_ENV_SIZE)
#else
#define	TOTAL_MALLOC_LEN	CFG_MALLOC_LEN
#endif

extern ulong uboot_end_data;
extern ulong uboot_end;

extern void ar7240_all_led_on(void);
extern void ar7240_all_led_off(void);

ulong monitor_flash_len;

const char version_string[] = U_BOOT_VERSION"  (" __DATE__ ")";

// Begin and End of memory area for malloc(), and current "brk"
static ulong mem_malloc_start;
static ulong mem_malloc_end;
static ulong mem_malloc_brk;

// The Malloc area is immediately below the monitor copy in DRAM
static void mem_malloc_init(void) {
	ulong dest_addr = CFG_MONITOR_BASE + gd->reloc_off;

	mem_malloc_end = dest_addr;
	mem_malloc_start = dest_addr - TOTAL_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

	memset((void *) mem_malloc_start, 0, mem_malloc_end - mem_malloc_start);
}

void *sbrk(ptrdiff_t increment) {
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;
	return ((void *) old);
}

static int display_banner(void) {
	printf("\n\n*****************************************\n*      %s      *\n*****************************************\n\n", version_string);
	return (0);
}

static int init_baudrate(void) {
	gd->baudrate = CONFIG_BAUDRATE;
	return (0);
}

/*
 * Breath some life into the board...
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t)(void);

init_fnc_t *init_sequence[] = { env_init, /* initialize environment */
								init_baudrate, /* initialze baudrate settings */
								console_init_f, /* initialize console */
								display_banner, /* say that we are here -> print baner */
								NULL, };

/*
 *
 * BOARD INITIALIZATION
 *
 */
void board_init_f(ulong bootflag) {
	gd_t gd_data, *id;
	bd_t *bd;
	init_fnc_t **init_fnc_ptr;
	ulong addr, addr_sp, len = (ulong) &uboot_end - CFG_MONITOR_BASE;
	ulong *s;
	char board_string[50];

	// Pointer is writable since we allocated a register for it.
	gd = &gd_data;

	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("": : :"memory");

	memset((void *) gd, 0, sizeof(gd_t));

	// loop trough init_sequence
	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang();
		}
	}

	// copies BOARD_CUSTOM_STRING to board_string
	checkboard(board_string);

	// print baner
	printf("%s\n\n", board_string);

	// count ram size and print it
	gd->ram_size = bootflag;
	puts("DRAM:  ");
	print_size(gd->ram_size, "\n");

	/*
	 * Now that we have DRAM mapped and working, we can
	 * relocate the code and continue running from DRAM.
	 */
	addr = CFG_SDRAM_BASE + gd->ram_size;

	/* We can reserve some RAM "on top" here.
	 */

	/* round down to next 4 kB limit.
	 */
	addr &= ~(4096 - 1);
	debug("Top of RAM usable for U-Boot at: %08lX\n", addr);

	/* Reserve memory for U-Boot code, data & bss
	 * round down to next 16 kB limit
	 */
	addr -= len;
	addr &= ~(16 * 1024 - 1);

	debug("Reserving %ldk for U-Boot at: %08lX\n", len >> 10, addr);

	/* Reserve memory for malloc() arena.
	 */
	addr_sp = addr - TOTAL_MALLOC_LEN;

	debug("Reserving %dk for malloc() at: %08lX\n", TOTAL_MALLOC_LEN >> 10, addr_sp);

	/*
	 * (permanently) allocate a Board Info struct
	 * and a permanent copy of the "global" data
	 */
	addr_sp -= sizeof(bd_t);
	bd = (bd_t *) addr_sp;
	gd->bd = bd;

	debug("Reserving %d Bytes for Board Info at: %08lX\n", sizeof(bd_t), addr_sp);

	addr_sp -= sizeof(gd_t);
	id = (gd_t *) addr_sp;

	debug("Reserving %d Bytes for Global Data at: %08lX\n", sizeof (gd_t), addr_sp);

	/* Reserve memory for boot params.
	 */
	addr_sp -= CFG_BOOTPARAMS_LEN;
	bd->bi_boot_params = addr_sp;

	debug("Reserving %dk for boot params() at: %08lX\n", CFG_BOOTPARAMS_LEN >> 10, addr_sp);

	/*
	 * Finally, we set up a new (bigger) stack.
	 *
	 * Leave some safety gap for SP, force alignment on 16 byte boundary
	 * Clear initial stack frame
	 */
	addr_sp -= 16;
	addr_sp &= ~0xF;
	s = (ulong *) addr_sp;
	*s-- = 0;
	*s-- = 0;
	addr_sp = (ulong) s;

	debug("Stack Pointer at: %08lX\n", addr_sp);

	/*
	 * Save local variables to board info struct
	 */
	bd->bi_memstart = CFG_SDRAM_BASE;	/* start of  DRAM memory */
	bd->bi_memsize  = gd->ram_size;		/* size  of  DRAM memory in bytes */
	bd->bi_baudrate = gd->baudrate;		/* Console Baudrate */

	memcpy(id, (void *) gd, sizeof(gd_t));

	relocate_code(addr_sp, id, addr);
	/* NOTREACHED - relocate_code() does not return */
}

/************************************************************************
 *
 * This is the next part if the initialization sequence: we are now
 * running from RAM and have a "normal" C environment, i. e. global
 * data can be written, BSS has been cleared, the stack size in not
 * that critical any more, etc.
 *
 ************************************************************************
 */
void board_init_r(gd_t *id, ulong dest_addr) {
	cmd_tbl_t *cmdtp;
	ulong size;
	extern void malloc_bin_reloc(void);
#ifndef CFG_ENV_IS_NOWHERE
	extern char * env_name_spec;
#endif
	bd_t *bd;
	int i;
	char *s;
	unsigned char buffer[6];

	gd = id;
	gd->flags |= GD_FLG_RELOC; /* tell others: relocation done */

	debug("Now running in RAM - U-Boot at: %08lX\n", dest_addr);

	gd->reloc_off = dest_addr - CFG_MONITOR_BASE;

	monitor_flash_len = (ulong) &uboot_end_data - dest_addr;

	/*
	 * We have to relocate the command table manually
	 */
	for (cmdtp = &__u_boot_cmd_start; cmdtp != &__u_boot_cmd_end; cmdtp++) {
		ulong addr;

		addr = (ulong) (cmdtp->cmd) + gd->reloc_off;

		cmdtp->cmd = (int (*)(struct cmd_tbl_s *, int, int, char *[])) addr;

		addr = (ulong) (cmdtp->name) + gd->reloc_off;
		cmdtp->name = (char *) addr;

		if (cmdtp->usage) {
			addr = (ulong) (cmdtp->usage) + gd->reloc_off;
			cmdtp->usage = (char *) addr;
		}
#ifdef	CFG_LONGHELP
		if (cmdtp->help) {
			addr = (ulong)(cmdtp->help) + gd->reloc_off;
			cmdtp->help = (char *)addr;
		}
#endif
	}

	/* there are some other pointer constants we must deal with */
#ifndef CFG_ENV_IS_NOWHERE
	env_name_spec += gd->reloc_off;
#endif

	/* configure available FLASH banks */
	size = flash_init();
	//puts("FLASH: ");
	//print_size(size, " ");

	//printf("(%d sectors)\n\n", CFG_MAX_FLASH_SECT);

	puts("\n\n");

	bd = gd->bd;
	bd->bi_flashstart = CFG_FLASH_BASE;
	bd->bi_flashsize = size;

#if CFG_MONITOR_BASE == CFG_FLASH_BASE
	bd->bi_flashoffset = monitor_flash_len; /* reserved area for U-Boot */
#else
	bd->bi_flashoffset = 0;
#endif

	/* initialize malloc() area */
	mem_malloc_init();
	malloc_bin_reloc();

	/* relocate environment function pointers etc. */
	env_relocate();

	/* board MAC address */
	memcpy(buffer, (void *)(AP121_OFFSET_FLASH_MAC_ADDRESS), 6);

	for (i = 0; i < 6; ++i) {
		bd->bi_enetaddr[i] = buffer[i];
	}

	/* IP Address */
	bd->bi_ip_addr = getenv_IPaddr("ipaddr");

	/** leave this here (after malloc(), environment and PCI are working) **/
	/* Initialize devices */
	devices_init();

	jumptable_init();

	/* Initialize the console (after the relocation and devices init) */
	console_init_r();
	/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

	/* Initialize from environment */
	if ((s = getenv("loadaddr")) != NULL) {
		load_addr = simple_strtoul(s, NULL, 16);
	}

#if (CONFIG_COMMANDS & CFG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif	/* CFG_CMD_NET */

#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r ();
#endif


	/* blink all available LEDs */
	printf("LED on during eth initialization...\n\n");
	ar7240_all_led_on();

#if (CONFIG_COMMANDS & CFG_CMD_NET)
	#if defined(CONFIG_NET_MULTI)
	//puts ("Net: ");
	#endif
	eth_initialize(gd->bd);
#endif

	ar7240_all_led_off();

	// command for netconsole start
	setenv("nc", "setenv stdin nc;setenv stdout nc;setenv stderr nc;version;");

	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop();
	}

	/* NOTREACHED - no way out of command loop except booting */
}

void hang(void) {
	puts("### ERROR ### Please RESET the board ###\n");
	for (;;)
		;
}
