#include <math.h>
#include "statPredict.h"




int confirm(float *space, int m1,int n1,				//预测空间 
			float observe_x,float observe_y,			//观测值x,y 
			int num,int select,							//num第几次观测 select选择对比空间的索引 (数组是从0开始的)
			float *pre_confirm_times)					//验证的指标 
{
	float dist = 0;
	if(m1 > 3)
		return 0;
	if(*(space + 2 * n1 + select) == 0) 					//说明要对比的数据没有存，这种情况只出现在第一次比较中 
		return 1;
	dist  = sqrt(pow(observe_x - *(space + 0 * n1 + select),2) + pow(observe_y - *(space +1 * n1 + select),2));
	if(dist > ENDURE_DIS)								//如果预测误差超过能够容忍误差 
	{
		*pre_confirm_times -=  0.2*(num - *(space + 2 * n1 + select));
		if(*pre_confirm_times < 0)
			 *pre_confirm_times = 0; 
	}
	else
		*pre_confirm_times +=  0.1*(num - *(space + 2 * n1 + select));
	return 1;
}




