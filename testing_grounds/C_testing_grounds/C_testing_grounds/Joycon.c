#pragma once
#include "Joycon.h"

//Returns new Joycon struct
joycon_t make_joycon(const struct hid_device_info* dev) {
	if (dev == NULL) {
		perror("Device cannot be NULL pointer.");
		exit(1);
	}

	joycon_t jc;

	if (dev->product_id == JOYCON_L_BT) {
		const char name[32] = "Joy-Con (L)";
		strcpy_s(jc.name, sizeof(jc.name), name);

		jc.type = JC_LEFT;
	}
	else if (dev->product_id == JOYCON_R_BT) {
		const char name[32] = "Joy-Con (R)";
		strcpy_s(jc.name, sizeof(jc.name), name);

		jc.type = JC_RIGHT;
	}

	jc.serial = _wcsdup(dev->serial_number);

	jc.handle = hid_open_path(dev->path);

	jc.global_packet_number = 0;

	if (jc.handle == NULL) {
		perror("Couldn't open device handle");
		exit(1);
	}

	return jc;
}

//Cleans up after using makeJoycon
void delete_joycon(joycon_t* jc) {
	if (jc != NULL) {
		hid_close(jc->handle);
		jc->handle = NULL;

		free(jc->serial);
		jc->serial = NULL;
	}
}

void send_command(joycon_t* jc, int command, uint8_t* data, int len) {
	unsigned char buf[COMMAND_BUF_SIZE];
	memset(buf, 0, COMMAND_BUF_SIZE);

	/* One byte for command and 63 bytes for data. */

	buf[0] = command;
	if (data && len != 0) {
		memcpy(buf + 1, data, len);
	}

	int result = hid_write(jc->handle, buf, len + 1);

	result = hid_read(jc->handle, buf, COMMAND_BUF_SIZE);

	if (data) {
		memcpy(data, buf, COMMAND_BUF_SIZE);
	}
}

void send_subcommand(joycon_t* jc, int command, int subcommand, uint8_t* data, int len) {
	uint8_t buf[COMMAND_BUF_SIZE];
	memset(buf, 0, COMMAND_BUF_SIZE);

	buf[0] = command; //0x01 for everything, 0x10 for rumble
	buf[1] = jc->global_packet_number;
	jc->global_packet_number++;
	jc->global_packet_number %= 0xF;

	uint8_t rumbleData[8] = {0x00, 0x01, 0x40, 0x40, 0x00, 0x01, 0x40, 0x40 };
	memcpy(buf + 2, rumbleData, 8);

	buf[10] = subcommand;
	memcpy(buf + 11, data, len);
	int result = hid_write(jc->handle, buf, COMMAND_BUF_SIZE);

	result = hid_read(jc->handle, buf, len);

	if (data) {
		memcpy(data, buf, len);
	}
	
}

int init_bt(joycon_t* jc) {

	unsigned char buf[0x40];
	memset(buf, 0, 0x40);

	hid_set_nonblocking(jc->handle, 0);


	printf("Enabling vibrations...\n");

	buf[0x0] = 0x01;
	send_subcommand(jc, 0x1, 0x48, buf, 1);
	for (int i = 0; i < 0x40; i++) {
		printf("%i ", buf[i]);
	}
	printf("\n");


	// Enable IMU data
	printf("Enabling IMU data...\n");
	buf[0] = 0x01; // Enabled
	send_subcommand(jc, 0x01, 0x40, buf, 1);
	for (int i = 0; i < 0x40; i++) {
		printf("%i ", buf[i]);
	}
	printf("\n");


	// Set input report mode (to push at 60hz)
	// x00	Active polling mode for IR camera data. Answers with more than 300 bytes ID 31 packet
	// x01	Active polling mode
	// x02	Active polling mode for IR camera data.Special IR mode or before configuring it ?
	// x21	Unknown.An input report with this ID has pairing or mcu data or serial flash data or device info
	// x23	MCU update input report ?
	// 30	NPad standard mode. Pushes current state @60Hz. Default in SDK if arg is not in the list
	// 31	NFC mode. Pushes large packets @60Hz
	printf("Set input report mode to 0x30...\n");
	buf[0] = 0x30;
	send_subcommand(jc, 0x01, 0x03, buf, 1);
	for(int i =0 ;i<0x40;i++){
		printf("%i ", buf[i]);
	}
	printf("\n");

}

void set_lights(joycon_t* jc, uint8_t bytefield) {
	uint8_t buff[1] = { bytefield };
	send_subcommand(jc, 0x01, 0x30, buff, 1);
}