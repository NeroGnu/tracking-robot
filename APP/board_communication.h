#ifndef __BOARD_COMMUNICATION_H
#define __BOARD_COMMUNICATION_H

#include "stm32f4xx.h"
#include "target_assign.h"

#define RE_TARGETNUM   (6)         //The number of target that video board send.
#define TARGETINFORLEN (4)         
#define BOARDRXLEN     (RE_TARGETNUM+RE_TARGETNUM*4+RE_TARGETNUM*4) //The the information format is : Efficiency(RE_TARGETNUM)(s8) XCoordinate(RE_TARGETNUM)(float)(The X coordinate of target.) Distance(RE_TARGETNUM)(float)(The target distance)


#ifdef TEST
extern char BCRxBuffer[RE_TARGETNUM][TARGETINFORLEN];
#elseif
extern char BCRxBuffer[BOARDRXLEN];
#endif

void BoardCommunication_Config(void);
int AccuracyTest(void);
void ProcessVideoResult(void);
void SerialTranslate(void);
#endif