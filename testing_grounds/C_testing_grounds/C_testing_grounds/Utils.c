#pragma once
#include "Utils.h"
#include "Joycon.h"
#include <stdio.h>


//TODO: Dopisac opcje odlaczania urzadzenia w trakcie, ogolnie do przebudowy
DWORD find_joycon(void* joycons)
{
	int found_devices = 0;
	while (true) {
		joycon_t* jc_arr = (joycon_t*)joycons;

		struct hid_device_info* devices = hid_enumerate(JOYCON_VENDOR, 0x0);
		struct hid_device_info* devices_head = devices;

		int i = 0;

		printf("[DISCOVERY] Looking for device...\n");

		while (devices != NULL) {
			i++;
			bool isNew = true;
			

			for (int j = 0; j < found_devices; j++) {
				if (devices->serial_number!= NULL && jc_arr[j].serial!=NULL && wcscmp(devices->serial_number, jc_arr[j].serial) == 0) {
					isNew = false;
				}
			}

			if (isNew) {
				printf("\n[DISCOVERY] Found new device<%i>...  \n------\n", found_devices);
				printf("Basic Joycon %i info:\n", i);

				wprintf(L"Manufacturer: %s\n", devices->manufacturer_string);
				wprintf(L"Product: %s\n", devices->product_string);
				wprintf(L"Serial number: %s\n", devices->serial_number);
				printf("Interface number: %i\n", devices->interface_number);

				jc_arr[i - 1] = make_joycon(devices);
				set_lights(&jc_arr[i - 1], 0x0F);
				found_devices++;
			}
			else if (devices->next == NULL) {
				printf("[DISCOVERY] No new devices found.\n");
			}
			
			devices = devices->next;
		}


		hid_free_enumeration(devices_head);
		Sleep(5000);
	}

	return 0;
}



void button_log_press(const char* btn_name) {
	printf("[CONTROL LOG] %s has been pressed.\n", btn_name);
}

void button_log_release(const char* btn_name) {
	printf("[CONTROL LOG] %s has been released.\n", btn_name);
}