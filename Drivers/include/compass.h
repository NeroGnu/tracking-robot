#ifndef __COMPASS_H__
#define __COMPASS_H__
#include "stm32f4xx.h"
#include "i2c.h"
#include <math.h>
//#include "public.h"

void COMP_Init(void);
void COMP_PreRead(void);
float COMP_Read(void);
//int Get_Angle(void);

#endif
