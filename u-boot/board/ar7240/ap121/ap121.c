#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include "ar7240_soc.h"

extern void	ar7240_ddr_initial_config(uint32_t refresh);
extern int	ar7240_ddr_find_size(void);
extern void	hornet_ddr_tap_init(void);

/*
void ar7240_usb_initial_config(void) {
	ar7240_reg_wr_nf(AR7240_USB_PLL_CONFIG, 0x0a04081e);
	ar7240_reg_wr_nf(AR7240_USB_PLL_CONFIG, 0x0804081e);
}

void ar7240_usb_otp_config(void) {
	unsigned int addr, reg_val, reg_usb;
	int time_out, status, usb_valid;

	for (addr = 0xb8114014;; addr -= 0x10) {
		status = 0;
		time_out = 20;

		reg_val = ar7240_reg_rd(addr);

		while ((time_out > 0) && (~status)) {
			if (((ar7240_reg_rd(0xb8115f18)) & 0x7) == 0x4) {
				status = 1;
			} else {
				status = 0;
			}
			time_out--;
		}

		reg_val = ar7240_reg_rd(0xb8115f1c);
		if ((reg_val & 0x80) == 0x80) {
			usb_valid = 1;
			reg_usb = reg_val & 0x000000ff;
		}

		if (addr == 0xb8114004) {
			break;
		}
	}

	if (usb_valid) {
		reg_val = ar7240_reg_rd(0xb8116c88);
		reg_val &= ~0x03f00000;
		reg_val |= (reg_usb & 0xf) << 22;
		ar7240_reg_wr(0xb8116c88, reg_val);
	}
}
*/

#define SETBITVAL(val, pos, bit) do {ulong bitval = (bit) ? 0x1 : 0x0; (val) = ((val) & ~(0x1 << (pos))) | ( (bitval) << (pos));} while (0)

void ar7240_led_toggle(void) {
	unsigned int gpio;

	gpio = ar7240_reg_rd(AR7240_GPIO_OUT);

#ifdef CONFIG_PID_MR302001
	gpio ^= 1 << GPIO_WPS_LED_BIT;
#endif

#ifdef CONFIG_PID_WR70301
	gpio ^= 1 << GPIO_SYS_LED_BIT;
#endif

#ifdef CONFIG_PID_MR304001
	gpio ^= 1 << GPIO_INTERNET_LED_BIT;
#endif

#ifdef CONFIG_PID_WR740N04
	gpio ^= 1 << GPIO_SYS_LED_BIT;
#endif

	ar7240_reg_wr(AR7240_GPIO_OUT, gpio);
}

void ar7240_all_led_on(void) {
	unsigned int gpio;

	gpio = ar7240_reg_rd(AR7240_GPIO_OUT);

#ifdef CONFIG_PID_MR302001
	SETBITVAL(gpio, GPIO_WPS_LED_BIT, GPIO_WPS_LED_ON);
	SETBITVAL(gpio, GPIO_INTERNET_LED_BIT, GPIO_INTERNET_LED_ON);
	SETBITVAL(gpio, GPIO_WLAN_LED_BIT, GPIO_WLAN_LED_ON);
	SETBITVAL(gpio, GPIO_ETH_LED_BIT, GPIO_ETH_LED_ON);
#endif

#ifdef CONFIG_PID_WR70301
	SETBITVAL(gpio, GPIO_SYS_LED_BIT, GPIO_SYS_LED_ON);
#endif

#ifdef CONFIG_PID_MR304001
	SETBITVAL(gpio, GPIO_INTERNET_LED_BIT, GPIO_INTERNET_LED_ON);
	SETBITVAL(gpio, GPIO_WLAN_LED_BIT, GPIO_WLAN_LED_ON);
	SETBITVAL(gpio, GPIO_ETH_LED_BIT, GPIO_ETH_LED_ON);
#endif

#ifdef CONFIG_PID_WR740N04
	SETBITVAL(gpio, GPIO_SYS_LED_BIT, GPIO_SYS_LED_ON);
	SETBITVAL(gpio, GPIO_WLAN_LED_BIT, GPIO_WLAN_LED_ON);
	SETBITVAL(gpio, GPIO_LAN1_LED_BIT, GPIO_LAN1_LED_ON);
	SETBITVAL(gpio, GPIO_LAN2_LED_BIT, GPIO_LAN2_LED_ON);
	SETBITVAL(gpio, GPIO_LAN3_LED_BIT, GPIO_LAN3_LED_ON);
	SETBITVAL(gpio, GPIO_LAN4_LED_BIT, GPIO_LAN4_LED_ON);
	SETBITVAL(gpio, GPIO_INTERNET_LED_BIT, GPIO_INTERNET_LED_ON);
	SETBITVAL(gpio, GPIO_QSS_LED_BIT, GPIO_QSS_LED_ON);
#endif

	ar7240_reg_wr(AR7240_GPIO_OUT, gpio);
}

void ar7240_all_led_off(void) {
	unsigned int gpio;

	gpio = ar7240_reg_rd(AR7240_GPIO_OUT);

#ifdef CONFIG_PID_MR302001
	SETBITVAL(gpio, GPIO_WPS_LED_BIT, !GPIO_WPS_LED_ON);
	SETBITVAL(gpio, GPIO_INTERNET_LED_BIT, !GPIO_INTERNET_LED_ON);
	SETBITVAL(gpio, GPIO_WLAN_LED_BIT, !GPIO_WLAN_LED_ON);
	SETBITVAL(gpio, GPIO_ETH_LED_BIT, !GPIO_ETH_LED_ON);
#endif

#ifdef CONFIG_PID_WR70301
	SETBITVAL(gpio, GPIO_SYS_LED_BIT, !GPIO_SYS_LED_ON);
#endif

#ifdef CONFIG_PID_MR304001
	SETBITVAL(gpio, GPIO_INTERNET_LED_BIT, !GPIO_INTERNET_LED_ON);
	SETBITVAL(gpio, GPIO_WLAN_LED_BIT, !GPIO_WLAN_LED_ON);
	SETBITVAL(gpio, GPIO_ETH_LED_BIT, !GPIO_ETH_LED_ON);
#endif

#ifdef CONFIG_PID_WR740N04
	SETBITVAL(gpio, GPIO_SYS_LED_BIT, !GPIO_SYS_LED_ON);
	SETBITVAL(gpio, GPIO_WLAN_LED_BIT, !GPIO_WLAN_LED_ON);
	SETBITVAL(gpio, GPIO_LAN1_LED_BIT, !GPIO_LAN1_LED_ON);
	SETBITVAL(gpio, GPIO_LAN2_LED_BIT, !GPIO_LAN2_LED_ON);
	SETBITVAL(gpio, GPIO_LAN3_LED_BIT, !GPIO_LAN3_LED_ON);
	SETBITVAL(gpio, GPIO_LAN4_LED_BIT, !GPIO_LAN4_LED_ON);
	SETBITVAL(gpio, GPIO_INTERNET_LED_BIT, !GPIO_INTERNET_LED_ON);
	SETBITVAL(gpio, GPIO_QSS_LED_BIT, !GPIO_QSS_LED_ON);
#endif

	ar7240_reg_wr(AR7240_GPIO_OUT, gpio);
}

// get button status
int ar7240_reset_button_status(void) {
	if (ar7240_reg_rd(AR7240_GPIO_IN) & (1 << GPIO_RST_BUTTON_BIT)) {
		return 1;
	} else {
		return 0;
	}
}

void ar7240_gpio_config(void) {
	/* Disable clock obs 
	 * clk_obs1(gpio13/bit8),  clk_obs2(gpio14/bit9), clk_obs3(gpio15/bit10),
	 * clk_obs4(gpio16/bit11), clk_obs5(gpio17/bit12)
	 * clk_obs0(gpio1/bit19), 6(gpio11/bit20)
	 */

	// TODO: ????
	ar7240_reg_wr(AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) & 0xEF84E0FB));

	/* Disable EJTAG functionality to enable GPIO functionality */
	ar7240_reg_wr(AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) | 0x8001));

	/* Set HORNET_BOOTSTRAP_STATUS BIT18 to ensure that software can control GPIO26 and GPIO27 */
	ar7240_reg_wr(HORNET_BOOTSTRAP_STATUS, (ar7240_reg_rd(HORNET_BOOTSTRAP_STATUS) | (0x1<<18)));

#ifdef CONFIG_PID_MR302001

	/* LED's GPIOs on MR3020:
	 *
	 * 0	=> WLAN
	 * 17	=> ETH
	 * 26	=> WPS
	 * 27	=> INTERNET
	 *
	 */

	/* set OE, added by zcf, 20110509 */
	ar7240_reg_wr(AR7240_GPIO_OE, (ar7240_reg_rd(AR7240_GPIO_OE) | 0xC020001));

	/* Disable clock obs, added by zcf, 20110509 */
	// TODO: ????
	//ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) & 0xffe7e07f));
#endif

#ifdef CONFIG_PID_WR70301

	/* LED's GPIOs on WR703N:
	 *
	 * 27	=> SYS
	 *
	 */

	/* set OE, added by zcf, 20110714 */
	ar7240_reg_wr(AR7240_GPIO_OE, (ar7240_reg_rd(AR7240_GPIO_OE) | 0x8000000));
#endif

#ifdef CONFIG_PID_MR304001

	/* LED's GPIOs on MR3040:
	 *
	 * 26	=> WLAN
	 * 17	=> ETH
	 * 27	=> INTERNET
	 *
	 */

	/* set OE, added by zcf, 20110509 */
	ar7240_reg_wr(AR7240_GPIO_OE, (ar7240_reg_rd(AR7240_GPIO_OE) | 0xC020000));

	/* Disable clock obs, added by zcf, 20110509 */
	// TODO: ????
	//ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) & 0xffe7e07f));
#endif

#ifdef CONFIG_PID_WR740N04

	/* LED's GPIOs on WR740Nv4:
	 *
	 * 0	=> WLAN
	 * 1	=> QSS
	 * 13	=> INTERNET
	 * 14	=> LAN1
	 * 15	=> LAN2
	 * 16	=> LAN3
	 * 17	=> LAN4
	 * 27	=> SYS
	 *
	 */

	/* set OE, added by zcf, 20110509 */
	ar7240_reg_wr(AR7240_GPIO_OE, (ar7240_reg_rd(AR7240_GPIO_OE) | 0x803E003));

	/* Disable clock obs, added by zcf, 20110509 */
	// TODO: ????
	//ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) & 0xffe7e07f));
#endif

}

int ar7240_mem_config(void) {

	/* Default tap values for starting the tap_init*/
	ar7240_reg_wr(AR7240_DDR_TAP_CONTROL0, CFG_DDR_TAP0_VAL);
	ar7240_reg_wr(AR7240_DDR_TAP_CONTROL1, CFG_DDR_TAP1_VAL);

	ar7240_gpio_config();
	ar7240_all_led_off();

	// TODO: check if this is necessary for USB initialization
	//ar7240_usb_initial_config();
	//ar7240_usb_otp_config();

	hornet_ddr_tap_init();

	// return memory size
	return (ar7240_ddr_find_size());
}

long int initdram(int board_type) {
	return (ar7240_mem_config());
}

int checkboard(char *board_string) {
	strcpy(board_string, BOARD_CUSTOM_STRING);
	return 0;
}

#ifdef CONFIG_SHOW_BOOT_PROGRESS
/*
 * Board specific code that enables you to show
 * the system's boot progress on some display (for
 * example, some LED's) on your board. At the moment,
 * the following checkpoints are implemented: SEE README!
 */
void show_boot_progress (int status) {
	// TODO: maybe we can use this functionality?
	//printf("\n\nBOOT PROGRESS: %d\n\n", status);
}
#endif
