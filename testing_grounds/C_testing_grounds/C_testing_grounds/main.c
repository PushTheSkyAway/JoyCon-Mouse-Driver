// C_testing_grounds.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
	Aktualnie aplikacja obsluguje nacisniecia przyciskow, zapalanie diod na padzie,
	potrafi odebrac i zdekodowac dokladna pozycje galek analogowych, ale jeszcze nie ma kalibracji

	Korzysta z biblioteki hidapi.h, ktora udostepnia API sterownika HID

*/

#include <stdio.h>
#include "../include/hidapi.h"
#include "Joycon.h"
#include "Utils.h"
#include "iocontroller.h"

int main(int argc, char** argv)
{
	//Kod automatycznie wywoljacy program z poziomu uzytkownika SYSTEM
	char acUserName[100];
	DWORD nUserName = sizeof(acUserName);

	if (GetUserNameA(acUserName, &nUserName)) {
		if (strcmp(acUserName, "Administrator") != 0 && strcmp(acUserName, "SYSTEM") != 0) {
			printf("Run this program as administrator!\n\You'll need psexec.exe sysinternals tool in your PATH.\n");
			system("pause");
			return 0;
		}
		if (strcmp(acUserName, "SYSTEM") != 0) {
			char command_buff[512] = { 0 };
			strcat_s(command_buff, 512, "psexec -s -i \"");
			strcat_s(command_buff, 512, argv[0]);
			strcat_s(command_buff, 512, "\"");

			system(command_buff);
			return 0;
		}
	}

	//===============KOD PROGRAMU==================

	int res = hid_init();

	joycon_t joycons[4] = { NULL, NULL, NULL, NULL };
	buttons_info_t buttons[4] = { NULL, NULL, NULL, NULL };
	buttons[0].STICK_POS = STICK_NEUTRAL;
	buttons[1].STICK_POS = STICK_NEUTRAL;
	buttons[2].STICK_POS = STICK_NEUTRAL;
	buttons[3].STICK_POS = STICK_NEUTRAL;

	//Watek wyszukujacy nowe urzadzenia co 5s.

	HANDLE discovery_thread = CreateThread(0, 0, find_joycon, (void*)joycons, 0, 0);

	uint8_t v = 10;
	bool isLdown = false;
	bool isRdown = false;
	for (;;) {
		for (int i = 0; i < 4; i++) {
			if (joycons[i].handle != NULL) {
				get_buttons_status(&joycons[i], &buttons[i]);

				//ustawienia czulosci
				if (buttons[i].SL) {
					v = v != 0 ? v - 1 : 0;
					button_log_press("SL");
				}
				if (buttons[i].SR) {
					v = v != 255 ? v + 1 : 255;
					button_log_press("SR");
				}

				//klikniecia
				if (buttons[i].RIGHT) {
					LeftDown();
					isLdown = true;
					button_log_press("RIGHT");
				}
				else {
					if (isLdown) {
						LeftUp();
						isLdown = false;
						button_log_release("RIGHT");
					}
				}

				if (buttons[i].DOWN) {
					RightDown();
					isRdown = true;
					button_log_press("DOWN");
				}
				else {
					if (isRdown) {
						RightUp();
						isRdown = false;
						button_log_release("DOWN");
					}
				}

				if (buttons[i].UP) {
					MiddleClick();
					button_log_press("UP");
				}

				if (buttons[i].R_L) {
					ScrollUp();
					button_log_press("R/L");
				}
				if (buttons[i].ZR_ZL) {
					ScrollDown();
					button_log_press("ZR/ZL");
				}

				if (buttons[i].MINUS_PLUS) {
					button_log_press("MINUS_PLUS");
					TerminateThread(discovery_thread, 0);	//Pamiec moze byc niepoprawnie czyszczona, ale watek przez 5s w kazdej petli dzialania jest posprzatany, wiec moze sie cos zgubic, ale prawdopodobienstwo jest raczej male.
					
					for (int i = 0; i < 2; i++) {
						delete_joycon(&joycons[i]);
					}
					res = hid_exit();
					
					ExitProcess(0);
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

	//free(devices);
	res = hid_exit();

	return 0;
}