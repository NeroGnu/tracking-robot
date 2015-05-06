#include "Timer2.h"

vu32 Timer2Counter = 0;
vu32 second_Counter = 0;

void Timer2Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_DeInit(TIM2);
	
	TIM_TimeBaseStructure.TIM_Period = 99;
  TIM_TimeBaseStructure.TIM_Prescaler = 83;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//T=(TIM_Period + 1) * (TIM_Prescaler + 1) / TIMxCLK = 100us
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	
}

void Delay_100us(unsigned long val)
{
	u32 TargetTick;
	TargetTick = Timer2Counter + val * 1;
	while(Timer2Counter < TargetTick){};
	
}

void Preemptive_Delay_100us(unsigned long val, vu8 *Flag)
{
	u32 TargetTick;
	TargetTick = Timer2Counter + val * 1;
	while(Timer2Counter < TargetTick)
	{
		if (0==*Flag)
		{
			break;
		}
	}
	
}