#pragma once

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include/hidapi.h"


#define JOYCON_VENDOR 0x057e


//Product IDs
#define JOYCON_L_BT 0x2006
#define JOYCON_R_BT 0x2007

#define COMMAND_BUF_SIZE 64

//Stick positions
#define STICK_UP 0
#define STICK_DOWN 4
#define STICK_LEFT 6
#define STICK_RIGHT 2
#define STICK_UP_LEFT 7
#define STICK_UP_RIGHT 1
#define STICK_DOWN_LEFT 5
#define STICK_DOWN_RIGHT 3
#define STICK_NEUTRAL 8



typedef enum {
	JC_LEFT,
	JC_RIGHT
} jc_type_t;


/*	
	char name[32]
	wchar_t *serial
	JC_type type
	hid_device* handle
*/
typedef struct {

	char name[32];
	wchar_t *serial;
	jc_type_t type;
	hid_device* handle;
	uint8_t global_packet_number;

} joycon_t;

typedef struct {
	bool SR;
	bool SL;
	bool R_L;
	bool ZR_ZL;
	bool MINUS_PLUS;
	bool UP;
	bool DOWN;
	bool LEFT;
	bool RIGHT;
	bool STICK;
	bool HOME_CAPTURE;
	uint8_t STICK_POS;
} buttons_info_t;



//Returns new Joycon struct
joycon_t make_joycon(const struct hid_device_info* dev);

//Cleans up after using makeJoycon
void delete_joycon(joycon_t* jc);


void send_command(joycon_t* jc, int command, uint8_t* data, int len);

void send_subcommand(joycon_t* jc, int command, int subcommand,  uint8_t* data, int len);
// b7 b6 b5 b4 <- lights flicker, b3 b2 b1 b0 <- lights on
void set_lights(joycon_t* jc, uint8_t bytefield);

void get_analog_stick_position(joycon_t* jc, uint8_t* X, uint8_t* Y);

uint8_t get_buttons_status(joycon_t* jc, buttons_info_t* btn_info_out);