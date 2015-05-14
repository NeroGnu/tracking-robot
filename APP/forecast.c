
#include <math.h>
#include "statPredict.h"
/*

int forecast(float *px,float *py,				//�������ǰ���ε�kaerman�˲���ֵ
			float p,											//(times Υ�����) / (endure_time �ܹ����̵ķ������)
			int start,											// ��ʼԤ���
			float confirm_times,								// ��֤�ġ�������
			float *out,int m2,int n2)							//���Ԥ������
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







