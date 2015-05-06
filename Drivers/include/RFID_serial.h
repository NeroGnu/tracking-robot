#ifndef __RFID_SERIAL_H
#define __RFID_SERIAL_H

#include "stm32f4xx.h"
#include <stdio.h>

#define IDLENTH 10

extern char RFID_ID[IDLENTH];

void Com6Init(void);
unsigned int IDtoInt(char* ID);

#endif

