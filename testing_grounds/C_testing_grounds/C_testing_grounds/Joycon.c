#pragma once
#include "Joycon.h"

//Returns new Joycon struct
Joycon make_joycon(const struct hid_device_info* dev) {
	if (dev == NULL) {
		perror("Device cannot be NULL pointer.");
		exit(1);
	}

	Joycon jc;

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

	if (jc.handle == NULL) {
		perror("Couldn't open device handle");
		exit(1);
	}

	return jc;
}

//Cleans up after using makeJoycon
void delete_joycon(Joycon* jc) {
	hid_close(jc->handle);
	jc->handle = NULL;

	free(jc->serial);
	jc->serial = NULL;
}

void send_command(Joycon* jc, int command, uint8_t* data, int len) {
	unsigned char buf[COMMAND_BUF_SIZE];
	memset(buf, 0, COMMAND_BUF_SIZE);

	/* One byte for command and 63 bytes for data. */

	buf[0] = command;
	if (data != NULL && len != 0) {
		memcpy(buf + 1, data, len);
	}

	int result = hid_write(jc->handle, buf, len + 1);

	result = hid_read(jc->handle, buf, COMMAND_BUF_SIZE);

	if (data) {
		memcpy(data, buf, 0x40);
	}
}