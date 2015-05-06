#include "RNG.h"


void RNG_Init(void)
{
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,ENABLE);
	RNG_Cmd(ENABLE);
	//RNG_ITConfig(ENABLE);
}

bool Bernoulli(float p)
{
	u32 rng, u32p;
	u32p=(u32)0xFFFFFFFF*p;
	while(RESET==RNG_GetFlagStatus(RNG_FLAG_DRDY));
	rng=RNG_GetRandomNumber();
	if (rng<=u32p)
		return TRUE;
	else
		return FALSE;
}