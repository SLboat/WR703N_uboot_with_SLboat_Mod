/*
 * This file contains the configuration parameters for the dbau1x00 board.
 */

#ifndef __AR7240_H
#define __AR7240_H

#ifndef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	3									/* autoboot after x seconds */
#endif

#define CONFIG_MENUPROMPT			"Hit any key to stop autobooting: %2d "
#define CONFIG_AUTOBOOT_PROMPT		"Autobooting in:\t%d s (type 'tpl' to run U-Boot console)\n\n"
#define CONFIG_AUTOBOOT_STOP_STR	"tpl"
#undef  CONFIG_AUTOBOOT_DELAY_STR
#define DEBUG_BOOTKEYS				0
#define CONFIG_BAUDRATE				115200
#define CFG_BAUDRATE_TABLE			{115200}

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP											/* undef to save memory      */
#define	CFG_PROMPT			"uboot> "							/* Monitor Command Prompt    */
#define	CFG_CBSIZE			256									/* Console I/O Buffer Size   */
#define	CFG_PBSIZE			(CFG_CBSIZE+sizeof(CFG_PROMPT)+24)	/* Print Buffer Size, TODO: def + 16 */
#define	CFG_MAXARGS			16									/* max number of command */
#define CFG_MALLOC_LEN		128*1024							// def: 128
#define CFG_BOOTPARAMS_LEN	128*1024							// def: 128
#define CFG_SDRAM_BASE		0x80000000							/* Cached addr */
#define	CFG_LOAD_ADDR		0x9F020000							/* default load address (81000000) */	// TODO: check this define
#define CFG_MEMTEST_START	0x80200000							// TODO: why not from CFG_SDRAM_BASE?
#define CFG_MEMTEST_END		0x83800000							// TODO: maybe we should change this?
#define CFG_RX_ETH_BUFFER   32									// TODO: maybe we could make it bigger (16 def)?

/*
 ** PLL Config for different CPU/DDR/AHB frequencies
 */
#define CFG_HZ						40000000

#define CFG_PLL_200_200_100			0x00
#define CFG_PLL_300_300_150			0x01
#define CFG_PLL_320_320_160			0x02
#define CFG_PLL_340_340_170			0x03
#define CFG_PLL_350_350_175			0x04
#define CFG_PLL_360_360_180			0x05
#define CFG_PLL_400_400_200			0x06
#define CFG_PLL_300_300_75			0x07
#define CFG_PLL_400_400_100			0x08
#define CFG_PLL_320_320_80			0x09
#define CFG_PLL_410_400_200			0x0a
#define CFG_PLL_420_400_200			0x0b
#define CFG_PLL_80_80_40			0x0c
#define CFG_PLL_64_64_32			0x0d
#define CFG_PLL_48_48_24			0x0e
#define CFG_PLL_32_32_16			0x0f
#define CFG_PLL_333_333_166			0x10
#define CFG_PLL_266_266_133			0x11
#define CFG_PLL_266_266_66			0x12
#define CFG_PLL_240_240_120			0x13
#define CFG_PLL_160_160_80			0x14
#define CFG_PLL_400_200_200			0x15
#define CFG_PLL_500_400_200			0x16
#define CFG_PLL_600_400_200			0x17
#define CFG_PLL_600_500_250			0x18
#define CFG_PLL_600_400_300			0x19
#define CFG_PLL_500_500_250			0x1a
#define CFG_PLL_600_350_175			0x1b
#define CFG_PLL_600_300_150			0x1c
#define CFG_PLL_600_550_1_1G_275	0x1d
#define CFG_PLL_600_500_1G_250		0x1e
#define CFG_PLL_533_400_200			0x1f
#define CFG_PLL_600_450_200			0x20
#define CFG_PLL_533_500_250			0x21
#define CFG_PLL_700_400_200			0x22
#define CFG_PLL_650_600_300			0x23
#define CFG_PLL_600_600_300			0x24
#define CFG_PLL_600_550_275			0x25
#define CFG_PLL_566_475_237			0x26
#define CFG_PLL_566_450_225			0x27
#define CFG_PLL_600_332_166			0x28
#define CFG_PLL_600_575_287			0x29
#define CFG_PLL_600_525_262			0x2a
#define CFG_PLL_566_550_275			0x2b
#define CFG_PLL_566_525_262			0x2c
#define CFG_PLL_600_332_200			0x2d
#define CFG_PLL_600_266_133			0x2e

#define CFG_PLL_412_412_206			0x2f
#define CFG_PLL_425_425_212			0x30
#define CFG_PLL_500_250_250			0x31
#define CFG_PLL_562_281_140			0x32
#define CFG_PLL_525_262_131			0x33
#define CFG_PLL_560_480_240			0x34

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE				32768
#define CFG_ICACHE_SIZE				65536
#define CFG_CACHELINE_SIZE			32

#endif	/* __CONFIG_H */
