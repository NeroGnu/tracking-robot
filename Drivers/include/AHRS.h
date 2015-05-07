#ifndef __AHRS_H
#define __AHRS_H

#include "stm32f4xx.h"

void AHRS_Com5Init(void);
bool AHRS_Check(void);
float AHRS_GetYaw(void);

extern char AHRS_Buffer[20];
#endif
