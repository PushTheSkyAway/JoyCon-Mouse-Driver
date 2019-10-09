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

	Joycon joycons[2];


	int i = 0;
	
	while (devices != NULL) {
		i++;

		printf("\nBasic Joycon %i info:\n", i);

		wprintf(L"Manufacturer: %s\n", devices->manufacturer_string);
		wprintf(L"Product: %s\n", devices->product_string);
		wprintf(L"Serial number: %s\n", devices->serial_number);
		printf("Interface number: %i\n", devices->interface_number);
		
		joycons[i - 1] = make_joycon(devices);
		devices = devices->next;
	}
	
	for (int i = 0; i < 2; i++) {
		delete_joycon(&joycons[i]);
	}

	res = hid_exit();

	return 0;
}
