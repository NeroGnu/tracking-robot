#ifndef __VEDIO_SERIAL_H
#define __VEDIO_SERIAL_H

#include "stm32f4xx.h"

#define RE_TARGETNUM   (6)         //The number of target that video board send.
#define TARGETINFORLEN (4)         
#define BOARDRXLEN     (RE_TARGETNUM+RE_TARGETNUM*4+RE_TARGETNUM*4) //The the information format is : Efficiency(RE_TARGETNUM)(s8) XCoordinate(RE_TARGETNUM)(float)(The X coordinate of target.) Distance(RE_TARGETNUM)(float)(The target distance)

extern char BCRxBuffer[BOARDRXLEN];

void VedioBoard_Com3Init(void);

#endif