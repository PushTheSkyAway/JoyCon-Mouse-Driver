#pragma once

#include <string.h>
#include <stdint.h>
#include "../include/hidapi.h"


#define JOYCON_VENDOR 0x057e

//Product IDs
#define JOYCON_L_BT 0x2006
#define JOYCON_R_BT 0x2007

#define COMMAND_BUF_SIZE 64

typedef enum {
	JC_LEFT,
	JC_RIGHT
} JC_type;


/*	
	char name[32]
	wchar_t *serial
	JC_type type
	hid_device* handle
*/
typedef struct {

	char name[32];
	wchar_t *serial;
	JC_type type;
	hid_device* handle;

} Joycon;



//Returns new Joycon struct
Joycon make_joycon(const struct hid_device_info* dev);

//Cleans up after using makeJoycon
void delete_joycon(Joycon* jc);


void send_command(Joycon* jc, int command, uint8_t* data, int len);