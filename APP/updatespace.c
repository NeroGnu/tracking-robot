
#include "statPredict.h"


int next_space;
void UpdateSpace(float *space,int m1,int n1,
				float *out,int m2,int n2,
				int num,int *flag)
{
	int space_min,space_max,save_min;
	int i,m;


	*(space + 2*n1 + (num-1)%FORECAST_LEN) = 0;
	space_min = ((num-1) * STEP+1) %FORECAST_LEN;
	space_max = (num * STEP +1)%FORECAST_LEN;
	save_min = (STEP-1) * num - STEP +1;

	if(space_max <= space_min)
		space_max = space_max + FORECAST_LEN;

	if(*flag == 1)
	{
		if(save_min >= FORECAST_LEN)
			save_min = FORECAST_LEN-1;
		*(space + 2*n1 + next_space) = num;
		*(space + 0*n1 + next_space) = *(out + 0*n2 + save_min);
		*(space + 1*n1 + next_space) = *(out + 1*n2 + save_min);
		next_space = (next_space + 1)%FORECAST_LEN;


	}
	else
	{
		for(i = space_min; i < space_max; i++)
		{
			if(i >= FORECAST_LEN)
				m = i - FORECAST_LEN;
			else
				m = i;

			if((*(space + 2*n1 + m)) > 0.00001)
			{
				*flag = 1;
				next_space = m;
				break;
			}
			*(space + 2*n1 + m) = num;
			*(space + 0*n1 + m) = *(out + 0*n2 + save_min + i - space_min);
			*(space + 1*n1 + m) = *(out + 1*n2 + save_min + i - space_min);

		}
	}

}


