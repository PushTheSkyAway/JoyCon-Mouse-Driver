// C_testing_grounds.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
	Aktualnie aplikacja obsluguje nacisniecia przyciskow, zapalanie diod na padzie,
	potrafi odebrac i zdekodowac dokladna pozycje galek analogowych, ale jeszcze nie ma kalibracji

	Korzysta z biblioteki hidapi.h, ktora udostepnia API sterownika HID
	   
*/

#include<stdio.h>
#include "../include/hidapi.h"
#include "Joycon.h"
#include "Utils.h"
#include "iocontroller.h"




int main()
{
	int res = hid_init();

	//Aktualnie wykrywa urzadzenia tylko raz, docelowo bedzie pilnowal podlaczonych urzadzen w innym watku
	struct hid_device_info* devices = hid_enumerate(JOYCON_VENDOR, 0x0);

	joycon_t joycons[2] = { NULL, NULL };
	buttons_info_t buttons[2] = { NULL, NULL };

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


	uint8_t v = 10;
	bool isLdown = false;
	bool isRdown = false;
	for (;;) {
		for (int i = 0; i < 2; i++) {
			if (joycons[i].handle != NULL) {
				
				get_buttons_status(&joycons[i], &buttons[i]);
				

				//ustawienia czulosci
				if (buttons[i].SL) {	
					v = v != 0 ? v - 1 : 0;
					
				}
				if (buttons[i].SR) {
					v = v != 255 ? v + 1 : 255;
				}


				//klikniecia
				if (buttons[i].RIGHT) {
					LeftDown();
					isLdown = true;
				}
				else {
					if (isLdown) {
						LeftUp();
						isLdown = false;
					}
				}

				if (buttons[i].DOWN) {
					RightDown();
					isRdown = true;

				}
				else {
					if (isRdown) {
						RightUp();
						isRdown = false;
					}
				}
				
				
				//Ruch mysza, poki co jedynie 8 kierunkow, ale niedlugo bedzie analogowy
				if (buttons[i].STICK_POS == STICK_UP || buttons[i].STICK_POS == STICK_UP_LEFT || buttons[i].STICK_POS == STICK_UP_RIGHT) {
					POINT pos;
					GetCursorPos(&pos);
					MoveCursor(pos.x, pos.y - v);
				}

				if (buttons[i].STICK_POS == STICK_DOWN || buttons[i].STICK_POS == STICK_DOWN_LEFT || buttons[i].STICK_POS == STICK_DOWN_RIGHT) {
					POINT pos;
					GetCursorPos(&pos);
					MoveCursor(pos.x, pos.y + v);
				}

				if (buttons[i].STICK_POS == STICK_LEFT || buttons[i].STICK_POS == STICK_UP_LEFT || buttons[i].STICK_POS == STICK_DOWN_LEFT) {
					POINT pos;
					GetCursorPos(&pos);
					MoveCursor(pos.x - v, pos.y);
				}

				if (buttons[i].STICK_POS == STICK_RIGHT || buttons[i].STICK_POS == STICK_UP_RIGHT || buttons[i].STICK_POS == STICK_DOWN_RIGHT) {
					POINT pos;
					GetCursorPos(&pos);
					MoveCursor(pos.x + v, pos.y);
				}




			}
		}



	}


	
	for (int i = 0; i < 2; i++) {
		delete_joycon(&joycons[i]);
	}

	free(devices);
	res = hid_exit();

	return 0;
}

