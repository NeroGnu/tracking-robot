#ifndef __TIMER2_H
#define __TIMER2_H
#include "stm32f4xx.h"

typedef union {
	u32 LastTick;
	u8 Split[4];
} Time;

extern vu32 Timer2Counter;
extern vu32 second_Counter;

void Timer2Init(void);
void Delay_100us(unsigned long val);
void Preemptive_Delay_100us(unsigned long val, vu8 *Flag);

#endif
