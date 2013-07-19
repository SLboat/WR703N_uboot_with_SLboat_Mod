/*
 * (C) Copyright 2013
 * Piotr Dymacz, Real Time Systems, piotr@realtimesystems.pl
 * Additional commands for TP-Link AP121 boards.
 *
 * See file CREDITS for list of people who contributed to U-Boot project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <command.h>

#ifdef AP121_OFFSET_FLASH_MAC_ADDRESS

/*
 * Show and change MAC address (changing only device ID)
 */
int do_print_mac(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]) {
	unsigned char buffer[6];

	// get MAC address from flash and print it
	memcpy(buffer, (void *) (AP121_OFFSET_FLASH_MAC_ADDRESS), 6);

	printf("Current MAC address stored in flash at offset 0x%X: ", AP121_OFFSET_FLASH_MAC_ADDRESS);
	printf("%02X:%02X:%02X:%02X:%02X:%02X\n\n", buffer[0] & 0xFF, buffer[1] & 0xFF, buffer[2] & 0xFF, buffer[3] & 0xFF, buffer[4] & 0xFF, buffer[5] & 0xFF);

	return 0;
}

U_BOOT_CMD(printmac, 1, 1, do_print_mac, "print MAC address stored in flash\n", NULL);

int do_set_mac(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]) {
	unsigned char *data_pointer;
	unsigned char buf[128];
	int i = 0, j = 0;

	// allow only 2 arg (command name + mac), second argument length should be 17 (xx:xx:xx:xx:xx:xx)
	if (argc != 2 || strlen(argv[1]) != 17) {
#ifdef  CFG_LONGHELP
		if(cmdtp->help != NULL) {
			printf("Usage:\n%s %s\n", cmdtp->name, cmdtp->help);
		} else {
			printf("Usage:\n%s %s\n", cmdtp->name, cmdtp->usage);
		}
#else
		printf("Usage:\n%s %s\n", cmdtp->name, cmdtp->usage);
#endif
		return 1;
	}

	// count ':'
	for(i = 0; i< 17; i++){
		if(argv[1][i] == ':'){
			j++;
		}
	}

	if(j != 5){
#ifdef  CFG_LONGHELP
		if(cmdtp->help != NULL) {
			printf("Usage:\n%s %s\n", cmdtp->name, cmdtp->help);
		} else {
			printf("Usage:\n%s %s\n", cmdtp->name, cmdtp->usage);
		}
#else
		printf("Usage:\n%s %s\n", cmdtp->name, cmdtp->usage);
#endif
		return 1;
	}

	// backup TP-Link block with MAC address from flash in RAM
	data_pointer = (unsigned char *) WEBFAILSAFE_UPLOAD_RAM_ADDRESS;

	if(!data_pointer){
		printf("## Error: couldn't allocate RAM for TP-Link data backup!\n");
		return 1;
	}

	memcpy((void *) data_pointer, (void *) AP121_OFFSET_FLASH_TPLINK_DATA, AP121_TPLINK_DATA_BLOCK_LENGHT);

	// store new MAC address in RAM
	for(i = 0; i < 6; i++){
		data_pointer[AP121_TPLINK_DATA_BLOCK_MAC_OFFSET + i] = simple_strtoul((char *)(argv[1] + i*3), NULL, 16);
	}

	// now we can erase flash and write data from RAM
	sprintf(buf,
			"erase 0x%lX +0x%lX; cp.b 0x%lX 0x%lX 0x%lX",
			AP121_OFFSET_FLASH_TPLINK_DATA,
			AP121_TPLINK_DATA_BLOCK_LENGHT,
			WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
			AP121_OFFSET_FLASH_TPLINK_DATA,
			AP121_TPLINK_DATA_BLOCK_LENGHT);

	printf("Executing: %s\n\n", buf);
	return run_command(buf, 0);
}

U_BOOT_CMD(setmac, 2, 1, do_set_mac, "save new MAC address in flash\n", "xx:xx:xx:xx:xx:xx\n\t- change MAC address stored in flash (xx - value in hex format)\n");

#endif /* AP121_OFFSET_FLASH_MAC_ADDRESS */

#ifdef AP121_OFFSET_FLASH_ROUTER_MODEL

/*
 * Show router model
 */
int do_print_model (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]) {
	unsigned char buffer[8];

	// get router model from flash and print it
	memcpy(buffer, (void *)(AP121_OFFSET_FLASH_ROUTER_MODEL), 8);

	printf("Router model stored in flash at offset 0x%X: ", AP121_OFFSET_FLASH_ROUTER_MODEL);
	printf("%02X%02X%02X%02X%02X%02X%02X%02X\n\n", buffer[0] & 0xFF, buffer[1] & 0xFF, buffer[2] & 0xFF, buffer[3] & 0xFF, buffer[4] & 0xFF, buffer[5] & 0xFF, buffer[6] & 0xFF, buffer[7] & 0xFF);

	return 0;
}

U_BOOT_CMD(printmodel, 1, 1, do_print_model, "print router model stored in flash\n", NULL);

#endif /* AP121_OFFSET_FLASH_ROUTER_MODEL */

#ifdef AP121_OFFSET_FLASH_PIN_NUMBER
#ifdef CONFIG_PID_MR302001 | CONFIG_PID_WR740N04
/*
 * Show pin number
 */
int do_print_pin (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]) {
	unsigned char buffer[9];

	// get pin number from flash and print it
	memcpy(buffer, (void *)(AP121_OFFSET_FLASH_PIN_NUMBER), 8);
	buffer[8] = 0;

	printf("Router pin number stored in flash at offset 0x%X: ", AP121_OFFSET_FLASH_PIN_NUMBER);
	printf("%s\n\n", buffer);
	//printf("%c%c%c%c%c%c%c%c\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);

	return 0;
}

U_BOOT_CMD(printpin, 1, 1, do_print_pin, "print WPS pin stored in flash\n", NULL);

#endif /* CONFIG_PID_MR302001 */
#endif /* AP121_OFFSET_FLASH_PIN_NUMBER */
