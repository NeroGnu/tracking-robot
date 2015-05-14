#ifndef __ARDUINO_SERIAL_H
#define __ARDUINO_SERIAL_H

#include "stm32f4xx.h"
#include <stdio.h>



extern char FEEDBACK_VALUE[];

void Com4Init(void);
int CmdSend(char* CmdStr, int Len, char End);

#endif

