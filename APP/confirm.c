#include <math.h>
#include "statPredict.h"




int confirm(float *space, int m1,int n1,				//Ԥ��ռ� 
			float observe_x,float observe_y,			//�۲�ֵx,y 
			int num,int select,							//num�ڼ��ι۲� selectѡ��Աȿռ������ (�����Ǵ�0��ʼ��)
			float *pre_confirm_times)					//��֤��ָ�� 
{
	float dist = 0;
	if(m1 > 3)
		return 0;
	if(*(space + 2 * n1 + select) == 0) 					//˵��Ҫ�Աȵ�����û�д棬�������ֻ�����ڵ�һ�αȽ��� 
		return 1;
	dist  = sqrt(pow(observe_x - *(space + 0 * n1 + select),2) + pow(observe_y - *(space +1 * n1 + select),2));
	if(dist > ENDURE_DIS)								//���Ԥ�������ܹ�������� 
	{
		*pre_confirm_times -=  0.2*(num - *(space + 2 * n1 + select));
		if(*pre_confirm_times < 0)
			 *pre_confirm_times = 0; 
	}
	else
		*pre_confirm_times +=  0.1*(num - *(space + 2 * n1 + select));
	return 1;
}




