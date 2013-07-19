/*
 * This file contains the configuration parameters for the dbau1x00 board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <configs/ar7240.h>
#include <config.h>

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 *-----------------------------------------------------------------------
 */
#define CFG_MAX_FLASH_BANKS		1
#define CFG_MAX_FLASH_SECT		4096	// 4 KB sectors in 16 MB flash

/*
 * We boot from this flash
 */
#define CFG_FLASH_BASE				0x9F000000
#define BOOTSTRAP_TEXT_BASE         CFG_FLASH_BASE
#define BOOTSTRAP_CFG_MONITOR_BASE  BOOTSTRAP_TEXT_BASE

/* 
 * The following #defines are needed to get flash environment right 
 */
#define	CFG_MONITOR_BASE	TEXT_BASE
#define	CFG_MONITOR_LEN		(192 << 10)		// TODO: ??

/*
 * Default bootargs for 4M flash size
 */
#undef CONFIG_BOOTARGS
#define	CONFIG_BOOTARGS     "console=ttyS0,115200 root=31:02 rootfstype=squashfs init=/sbin/init mtdparts=ar7240-nor0:128k(u-boot),3904(firmware),64k(ART)"

#undef CFG_PLL_FREQ
#undef CFG_HZ
#undef CPU_PLL_CONFIG_VAL1
#undef CPU_CLK_CONTROL_VAL2

// CPU-RAM-AHB frequency setting
#define CFG_PLL_FREQ    CFG_PLL_400_400_200

/*
 * MIPS32 24K Processor Core Family Software User's Manual
 *
 * 6.2.9 Count Register (CP0 Register 9, Select 0)
 * The Count register acts as a timer, incrementing at a constant
 * rate, whether or not an instruction is executed, retired, or
 * any forward progress is made through the pipeline.  The counter
 * increments every other clock, if the DC bit in the Cause register
 * is 0.
 *
 * Since the count is incremented every other tick, divide by 2
 * XXX derive this from CFG_PLL_FREQ
 */


/*
 * CPU_PLL_DITHER_FRAC_VAL
 *
 * Value written into CPU PLL Dither FRAC Register (PLL_DITHER_FRAC)
 *
 * bits	0..9	NFRAC_MAX	=>	1000 (0x3E8)
 * bits	10..13	NFRAC_MIN	=>	0 (minimum value is used)
 * bits	20..29	NFRAC_STEP	=>	1
 *
 */
#define CPU_PLL_DITHER_FRAC_VAL		0x001003e8

/*
 * CPU_PLL_SETTLE_TIME_VAL
 *
 * Value written into CPU Phase Lock Loop Configuration Register 2 (CPU_PLL_CONFIG2)
 *
 * bits	0..11	SETTLE_TIME	=>	580 (0x352)
 *
 */
#define CPU_PLL_SETTLE_TIME_VAL		0x00000352

/*
 * CPU_CLK_CONTROL_VAL1
 * CPU_CLK_CONTROL_VAL2
 *
 * Values written into CPU Clock Control Register CLOCK_CONTROL
 * with PLL bypass disabled
 *
 * bits	2		(1bit)	BYPASS (Bypass PLL. This defaults to 1 for test purposes. Software must enable the CPU PLL for normal operation and then set this bit to 0)
 * bits	5..6	(2bit)	CPU_POST_DIV	=>	0	(DEFAULT, Ratio = 1)
 * bits	10..11	(2bit)	DDR_POST_DIV 	=>	0	(DEFAULT, Ratio = 1)
 * bits	15..16	(2bit)	AHB_POST_DIV	=>	1	(DEFAULT, Ratio = 2)
 *
 */

/*
 * CPU_PLL_CONFIG_VAL1
 * CPU_PLL_CONFIG_VAL2
 *
 * In CPU_PLL_CONFIG_VAL1 bit 30 is set (CPU_PLLPWD = 1 -> power down control for CPU PLL)
 * In CPU_PLL_CONFIG_VAL2 bit 30 is unset
 *
 * Values written into CPU Phase Lock Loop Configuration (CPU_PLL_CONFIG)
 *
 * bits 10..15	(6bit)	DIV_INT	(The integer part of the DIV to CPU PLL)			=>	32	(0x20)
 * bits 16..20	(5bit)	REFDIV	(Reference clock divider)							=>	1	(0x1)		[doesn't start at valuse different than 1 (maybe need to change other dividers?)]
 * bits 21		(1bit)	RANGE	(Determine the VCO frequency range of the CPU PLL)	=>	0	(0x0)		[doesn't have impact on clock values]
 * bits 23..25	(3bit)	OUTDIV	(Define the ratio betwee VCO output and PLL output	=>	1	(0x1)
 * 								VCOOUT * (1/2^OUTDIV) = PLLOUT)
 */

/*
 * = PLL CALCULATION (guess) =============
 * PLL = (25 MHz * DIV_INT) / (2 ^ OUTDIV)
 *
 * CPU = PLL / CPU_POST_DIV
 * DDR = PLL / DDR_POST_DIV
 * AHB = PLL / AHB_POST_DIV
 *
 */

/*
 * AR7240_SPI_CONTROL
 *
 * Value written into SPI Control (SPI_CONTROL) register
 *
 * bits	0..5	(6bit)	CLOCK_DIVIDER	(Specifies the clock divider setting. Actual clock frequency would be (AHB_CLK / ((CLOCK_DIVIDER+1)*2)) )
 * bits	6		(1bit)	REMAP_DISABLE	(Remaps 4 MB space over unless explicitly disabled by setting this bit to 1. If set to 1, 16 MB is accessible.)
 *
 */

#if (CFG_PLL_FREQ == CFG_PLL_400_400_200)

	#define CFG_HZ					(400000000/2)

	// CPU_DIV = 1, RAM_DIV = 1, AHB_DIV = 2
	#define CPU_CLK_CONTROL_VAL1	0x00008004
	#define CPU_CLK_CONTROL_VAL2	0x00008000

	// DIV_INT	= 32	(25 MHz * 32/2 = 400 MHz)
	// REFDIV	= 1
	// RANGE	= 0
	// OUTDIV	= 1
	#define CPU_PLL_CONFIG_VAL1		0x40818000
	#define CPU_PLL_CONFIG_VAL2		0x00818000

	// CLOCK_DIVIDER = 2 (SPI clock = 200 / 6 ~ 33,3 MHz)
	#define AR7240_SPI_CONTROL		0x42

#elif (CFG_PLL_FREQ == CFG_PLL_412_412_206)

	#define CFG_HZ					(412500000/2)

	// CPU_DIV = 1, RAM_DIV = 1, AHB_DIV = 2
	#define CPU_CLK_CONTROL_VAL1	0x00008004
	#define CPU_CLK_CONTROL_VAL2	0x00008000

	// DIV_INT	= 33	(25 MHz * 33/2 = 412,5 MHz)
	// REFDIV	= 1
	// RANGE	= 0
	// OUTDIV	= 1
	#define CPU_PLL_CONFIG_VAL1		0x40818400
	#define CPU_PLL_CONFIG_VAL2		0x00818400

	// CLOCK_DIVIDER = 2 (SPI clock = 206,25 / 6 ~ 34,4 MHz)
	#define AR7240_SPI_CONTROL		0x42

#elif (CFG_PLL_FREQ == CFG_PLL_425_425_212)

	#define CFG_HZ					(425000000/2)

	// CPU_DIV = 1, RAM_DIV = 1, AHB_DIV = 2
	#define CPU_CLK_CONTROL_VAL1	0x00008004
	#define CPU_CLK_CONTROL_VAL2	0x00008000

	// DIV_INT	= 34	(25 MHz * 34/2 = 425 MHz)
	// REFDIV	= 1
	// RANGE	= 0
	// OUTDIV	= 1
	#define CPU_PLL_CONFIG_VAL1		0x40818800
	#define CPU_PLL_CONFIG_VAL2		0x00818800

	// CLOCK_DIVIDER = 2 (SPI clock = 212,5 / 6 ~ 35,4 MHz)
	#define AR7240_SPI_CONTROL		0x42

#elif (CFG_PLL_FREQ == CFG_PLL_500_250_250)

	#define CFG_HZ					(500000000/2)

	// CPU_DIV = 1, RAM_DIV = 2, AHB_DIV = 2
	#define CPU_CLK_CONTROL_VAL1	0x00008404
	#define CPU_CLK_CONTROL_VAL2	0x00008400

	// DIV_INT	= 40	(25 MHz * 40/2 = 500 MHz)
	// REFDIV	= 1
	// RANGE	= 0
	// OUTDIV	= 1
	#define CPU_PLL_CONFIG_VAL1		0x4081A000
	#define CPU_PLL_CONFIG_VAL2		0x0081A000

	// CLOCK_DIVIDER = 3 (SPI clock = 250 / 8 ~ 31,3 MHz)
	#define AR7240_SPI_CONTROL		0x43

#elif (CFG_PLL_FREQ == CFG_PLL_562_281_140)

	#define CFG_HZ					(562500000/2)

	// CPU_DIV = 1, RAM_DIV = 2, AHB_DIV = 4
	#define CPU_CLK_CONTROL_VAL1	0x00018404
	#define CPU_CLK_CONTROL_VAL2	0x00018400

	// DIV_INT	= 45	(25 MHz * 45/2 = 562,5 MHz)
	// REFDIV	= 1
	// RANGE	= 0
	// OUTDIV	= 1
	#define CPU_PLL_CONFIG_VAL1		0x4081B400
	#define CPU_PLL_CONFIG_VAL2		0x0081B400

	// CLOCK_DIVIDER = 1 (SPI clock = 140,625 / 4 ~ 35,2 MHz)
	#define AR7240_SPI_CONTROL		0x41

#elif (CFG_PLL_FREQ == CFG_PLL_525_262_131)

	#define CFG_HZ					(525000000/2)

	// CPU_DIV = 1, RAM_DIV = 2, AHB_DIV = 4
	#define CPU_CLK_CONTROL_VAL1	0x00018404
	#define CPU_CLK_CONTROL_VAL2	0x00018400

	// DIV_INT	= 42	(25 MHz * 42/2 = 525 MHz)
	// REFDIV	= 1
	// RANGE	= 0
	// OUTDIV	= 1
	#define CPU_PLL_CONFIG_VAL1		0x4081A800
	#define CPU_PLL_CONFIG_VAL2		0x0081A800

	// CLOCK_DIVIDER = 1 (SPI clock = 131 / 4 ~ 32,8 MHz)
	#define AR7240_SPI_CONTROL		0x41

#endif

/*
 * Cache lock for stack
 */
#define CFG_INIT_SP_OFFSET		0x1000

/* Address and size of Primary Environment Sector	*/
#undef  CFG_ENV_IS_IN_FLASH
#define CFG_ENV_IS_NOWHERE		1
#define CFG_ENV_ADDR			0x9f040000
#define CFG_ENV_SIZE			0x10000

/* Some env settings */
#define CONFIG_BOOTCOMMAND "bootm 0x9F020000"
#define CONFIG_IPADDR		192.168.1.1
#define CONFIG_SERVERIP		192.168.1.2
//#define CONFIG_ETHADDR		0xba:0xbe:0xfa:0xce:0x07:0x41

// TODO: ping functionality? CFG_CMD_PING, CFG_CMD_DHCP, CFG_CMD_BOOTD?
#define CONFIG_COMMANDS (CFG_CMD_MEMORY | CFG_CMD_FLASH | CFG_CMD_NET | CFG_CMD_BOOTD | CFG_CMD_RUN)

// netconsole
#define CONFIG_NETCONSOLE

/* DDR init values */
#if CONFIG_40MHZ_XTAL_SUPPORT
	#define CFG_DDR_REFRESH_VAL	0x4270
#else
	#define CFG_DDR_REFRESH_VAL	0x4186
#endif

#define CFG_DDR_CONFIG_VAL		0x7fbc8cd0
#define CFG_DDR_MODE_VAL_INIT	0x133

#ifdef LOW_DRIVE_STRENGTH
	#define CFG_DDR_EXT_MODE_VAL	0x2
#else
	#define CFG_DDR_EXT_MODE_VAL	0x0
#endif

#define CFG_DDR_MODE_VAL	0x33
#define CFG_DDR_TRTW_VAL	0x1f
#define CFG_DDR_TWTR_VAL	0x1e

//#define CFG_DDR_CONFIG2_VAL			0x99d0e6a8	// HORNET 1.0
#define CFG_DDR_CONFIG2_VAL				0x9dd0e6a8	// HORNET 1.1
#define CFG_DDR_RD_DATA_THIS_CYCLE_VAL	0x00ff
#define CFG_DDR_TAP0_VAL				0x8
#define CFG_DDR_TAP1_VAL				0x9

/* DDR2 Init values */
#define CFG_DDR2_EXT_MODE_VAL			0x402

#define CONFIG_NET_MULTI

#define CONFIG_AG7240_SPEPHY /* choose eth1 first for tftpboot interface added by ZJin, 110328 */

/*-----------------------------------------------------------------------
 * Web Failsafe configuration
 */
#define WEBFAILSAFE_UPLOAD_RAM_ADDRESS			0x80080000		// TODO: need to check why we have problem storing downloaded data from the beginning of RAM
#define WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS		CFG_FLASH_BASE
#define WEBFAILSAFE_UPLOAD_KERNEL_ADDRESS		WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS + 0x20000
#define WEBFAILSAFE_UPLOAD_UBOOT_SIZE_IN_BYTES	(64 * 1024)

// progress state info
#define WEBFAILSAFE_PROGRESS_START				0
#define WEBFAILSAFE_PROGRESS_TIMEOUT			1
#define WEBFAILSAFE_PROGRESS_UPLOAD_READY		2
#define WEBFAILSAFE_PROGRESS_UPGRADE_READY		3
#define WEBFAILSAFE_PROGRESS_UPGRADE_FAILED		4

// update type
#define WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE		0
#define WEBFAILSAFE_UPGRADE_TYPE_UBOOT			1
#define WEBFAILSAFE_UPGRADE_TYPE_ART			2

/*-----------------------------------------------------------------------*/

// TODO: ??
#define CFG_ATHRS26_PHY				1
#define CFG_FAULT_ECHO_LINK_DOWN	1
#define CFG_AG7240_NMACS			2
#define CFG_GMII					0
#define CFG_MII0_RMII				1
#define CFG_BOOTM_LEN				(16 << 20) /* 16 MB */
#define ATHEROS_PRODUCT_ID			138

#undef DEBUG
#define milisecdelay(_x)			udelay((_x) * 1000)

/* MAC address, model and PIN number offsets in FLASH */
#define AP121_TPLINK_DATA_BLOCK_LENGHT		0x10000
#define AP121_TPLINK_DATA_BLOCK_MAC_OFFSET	0x0FC00

#define AP121_OFFSET_FLASH_TPLINK_DATA	CFG_FLASH_BASE + 0x10000
#define AP121_OFFSET_FLASH_MAC_ADDRESS	CFG_FLASH_BASE + 0x1FC00
#define AP121_OFFSET_FLASH_ROUTER_MODEL	CFG_FLASH_BASE + 0x1FD00
#define AP121_OFFSET_FLASH_PIN_NUMBER	CFG_FLASH_BASE + 0x1FE00

#include <cmd_confdefs.h>

#endif	/* __CONFIG_H */
