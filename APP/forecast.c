
#include <math.h>
#include "statPredict.h"
/*

int forecast(float *px,float *py,				//保存的是前几次的kaerman滤波的值
			float p,											//(times 违规次数) / (endure_time 能够容忍的犯规次数)
			int start,											// 开始预测点
			float confirm_times,								// 验证的“次数”
			float *out,int m2,int n2)							//输出预测数据
*/

int forecast(float *px,float *py,float p,int start,	float confirm_times,float *out,int m2,int n2)
{
	int i;



	for(i = 0; i < FORECAST_LEN; i++)
	{
		
		*(out + 0*n2 + i) = px[1] *(start + i+1) + px[0];
		*(out + 1*n2 + i) = py[1] *(start + i+1) + py[0];
		*(out + 2*n2 + i) = (1-pow(p,2))*exp(-(i+1)/(pow(confirm_times+1,2)));
		
		
//		printf("px[0] = %f\r\n",px[0]);
//		printf("*(out + 0*n2 + i) = %f\r\n",*(out + 0*n2 + i));
	}

	return 1;

}







