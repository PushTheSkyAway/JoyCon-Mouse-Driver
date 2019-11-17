// C_testing_grounds.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include<stdio.h>
#include "../include/hidapi.h"
#include "Joycon.h"
#include "Utils.h"
#include <Windows.h>

void LeftClick()
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));

	// left up
	ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));
}

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

	/*uint8_t data[12];
	data[0] = 0x30;
	send_subcommand(&joycons[0], 0x01, 0x03, data, 1);
	set_lights(&joycons[0], 0x96);
	while (1) {
		uint8_t data[1];
		memset(data, 0, 1);
		send_subcommand(&joycons[i - 1], 0x01, 0x00, data, 1);

		for (int i = 0; i < 16; i++) {
			printf("%02X ", data[i]);
		}
		printf("\r");
	}*/

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD CursorPosition;

	int X = 0;
	int Y = 0;
	int lastX = 900;
	int lastY = 0;
	int vX = 0;
	int vY = 0;
	while (1) {
		get_analog_stick_position(&joycons[0], &X, &Y);

		printf("X: %i, Y: %i\r", X, Y);
		vX = -1 * (X - 2255);
		vY = -1 * (Y - 1825);

		if (abs(X - 2255) > 225) {
			lastX += vX / 35;
		}

		if (abs(Y - 1825) > 182) {
			lastY += vY / 35;
		}

		SetCursorPos(lastX, lastY);
		if (get_buttons_status(&joycons[0]) == 1) {
			LeftClick();
		}
	}

	for (int i = 0; i < 2; i++) {
		delete_joycon(&joycons[i]);
	}

	free(devices);
	res = hid_exit();

	return 0;
}

