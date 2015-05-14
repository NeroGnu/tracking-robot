
#include "statPredict.h"



//返回值表示目标是否机动，1：机动  2：不机动 

int Jugemotor(float *estimate_value,int m1,int n1,				//保存的是前几次的kaerman滤波的值 
			  float observe_x,float observe_y,			//观测值x,y 
			  int *times,								//犯规次数
			  int *Up_down,								//Up_down:  Up = 1;  down = 0;
			  float *p_x,float *p_y						//x和y的最小二乘法 
			)
{
	float delta = 0.0f;
	delta = p_y[1]/p_x[1] *observe_x - p_y[1]/p_x[1] * p_x[0] + p_y[0] - observe_y;				//delta = (k2/k1)*x - (k2/k1)*b1 +b2 -y
	
	if(delta >= 0)
	{
		if(*Up_down == 0 && delta >= ENDURE_DIS) 
			*times += 1;
		else
			*times -= 1;
		*Up_down = 0;
	}
	else
	{
		if(*Up_down == 1 && -delta >= ENDURE_DIS)
			*times += 1;
		else
			*times -= 1;
		*Up_down = 0;
	}
	
	if(*times < 0)
		*times = 0;
		
	if(*times > ENDURE_TIME)
		return 1;													
	else
		return 0;	
	
}


