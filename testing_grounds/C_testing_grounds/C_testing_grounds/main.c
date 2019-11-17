// C_testing_grounds.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include<stdio.h>
#include "../include/hidapi.h"
#include "Joycon.h"
#include "Utils.h"




int main()
{
	int res = hid_init();

	struct hid_device_info* devices = hid_enumerate(JOYCON_VENDOR, 0x0);

	joycon_t joycons[2] = { NULL, NULL };


	int i = 0;
	
	while (devices != NULL) {
		i++;

		printf("\nBasic Joycon %i info:\n", i);

		wprintf(L"Manufacturer: %s\n", devices->manufacturer_string);
		wprintf(L"Product: %s\n", devices->product_string);
		wprintf(L"Serial number: %s\n", devices->serial_number);
		printf("Interface number: %i\n", devices->interface_number);
		
		joycons[i - 1] = make_joycon(devices);
		//init_bt(joycons[i - 1]);
		

		devices = devices->next;
	}
	
	uint8_t data[12];
	data[0] = 0x30;
	send_subcommand(&joycons[0], 0x01, 0x03, data, 1);
	set_lights(&joycons[0], 0x96);
	while (1) {
		uint8_t data[1] = { 0x70 };
		//memset(data, 0, 1);
		//send_subcommand(&joycons[i - 1], 0x01, 0x00, data, 1);

		for (int i = 0; i < 16; i++) {
			printf("%02X ", data[i]);
		}
		printf("\r");
	}


	for (int i = 0; i < 2; i++) {
		delete_joycon(&joycons[i]);
	}

	free(devices);
	res = hid_exit();

	return 0;
}
