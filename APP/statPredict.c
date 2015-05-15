
#include "statPredict.h"
#include "array.h"
#include "ployfit.h"



int count = 0;
float space[3][FORECAST_LEN] = {0.0f};
float ployfit_value[2][PLOTFIT_LEN];
float confirm_times = 0.1;
int times = 1,up_down = 1,Isfull = 0,times_of_pos = 0;
float pre_Xkf[4][1] = {0.0f};
float pre_P[4][4] = {{1,0,0,0},
                      {0,1,0,0},
                      {0,0,1,0},
                      {0,0,0,1}
					  };

void State_prediction(time_pos *Z,Pred *out_predict,int *flag)
{
  float p1[2],p2[2];
	//float out[3][FORECAST_LEN] = {0.0f};
	float tmp[PLOTFIT_LEN];
  float out[3][FORECAST_LEN] = {0.0f};

	float eye_4[4][4] = {{1,0,0,0},
					  {0,1,0,0},
					  {0,0,1,0},
					  {0,0,0,1}
					  };

	float now_Z[2][1];							//????????  (??) 2?1?
	float pre_R[2][2] = {{100,0},{0,100}};						//?????????(??) 	2?2?
	float now_Xkf[4][1];									// ????????  (??) 4?1?

	float now_P[4][4];									//????????(??)  4?4?


	int j,k;


	if(count < PLOTFIT_LEN)
    {
        *flag = 0;


        ployfit_value[0][count] = (*Z).x;
        ployfit_value[1][count] = (*Z).y;

        count++;
        times_of_pos++;
        return;
    }
    times_of_pos++;

    k = times_of_pos - PLOTFIT_LEN;

    for(j = 0; j < PLOTFIT_LEN; j++)
    {
        tmp[j] = (float) (j+k);
				//printf("tmp = %f,ployfit_value = %f\r\n",tmp[j],ployfit_value[0][j] );
    }
    polyfit(PLOTFIT_LEN,tmp,ployfit_value[0],1,p1);
    polyfit(PLOTFIT_LEN,tmp,ployfit_value[1],1,p2);

//		printf("P1 = %f\r\n",p1[0]);
    if(count == PLOTFIT_LEN)
    {
        array_set_value(pre_Xkf[0],4,1,NULL,0,0,SINGLE,p1[0] + (times_of_pos-1)*p1[1]);
        array_set_value(pre_Xkf[0],4,1,NULL,1,0,SINGLE,p1[1]);
        array_set_value(pre_Xkf[0],4,1,NULL,2,0,SINGLE,p2[0] + (times_of_pos-1)*p2[1]);
        array_set_value(pre_Xkf[0],4,1,NULL,3,0,SINGLE,p2[1]);

    }
    count++;

    

    now_Z[0][0] = (*Z).x;
    now_Z[1][0] = (*Z).y;



//		printf("\r\n");
//		printf("pre_P\r\n");
//		print_array(pre_P[0],4,4);
//		printf("\r\n");

    Line_kalman(pre_Xkf[0],						//???????? (??)4?1?
				pre_P[0],						//???????? (??) 4?4?
				now_Z[0],							//????????  (??) 2?1?
				pre_R[0],						//?????????(??) 	2?2?
				now_Xkf[0],									// ????????  (??) 4?1?
				NULL,									//?????????(??) 2?2?
				now_P[0],									//????????(??)  4?4?
				NULL, 0					//?????????(??)
				);

//		print_array(now_Xkf[0],4,1);
//		printf("\r\n");
		//array_get_column(Z[0],2,3*N,now_Z[0],2,1,i);
		array_equal(now_Xkf[0],4,1,pre_Xkf[0],4,1);
		array_equal(now_P[0],4,4,pre_P[0],4,4);
		//array_set_value(Xkf[0],4,3*N,now_Xkf[0],4,1,COLUMN,i);

        (*Z).x = now_Xkf[0][0];
        (*Z).y = now_Xkf[2][0];



        //????
		for(j = 0; j < PLOTFIT_LEN - 1; j++)
		{
			ployfit_value[0][j] = ployfit_value[0][j+1];
			ployfit_value[1][j] = ployfit_value[1][j+1];
		}
		ployfit_value[0][PLOTFIT_LEN - 1] = now_Xkf[0][0];
		ployfit_value[1][PLOTFIT_LEN - 1] = now_Xkf[2][0];
		

        if(Jugemotor(ployfit_value[0],2,PLOTFIT_LEN,				//????????kaerman????
			  now_Z[0][0],now_Z[1][0],			//???x,y
			  &times,								//????
			  &up_down,								//Up_down:  Up = 1;  down = 0;
			  p1,p2						//x?y??????
			))
			{
				count = 0;

				array_equal(eye_4[0],4,4,pre_P[0],4,4);
				times = 0;
				confirm_times = 0;
                zeros_array(space[0],3,FORECAST_LEN);
                Isfull = 0;
                *flag = 0;

                return ;
			}

			confirm(space[0], 3,FORECAST_LEN,now_Z[0][0],now_Z[1][0],count-PLOTFIT_LEN,(count-PLOTFIT_LEN-1)%FORECAST_LEN,&confirm_times);


			forecast(p1,p2,times/ENDURE_TIME,times_of_pos,confirm_times,out[0],3,FORECAST_LEN);

			UpdateSpace(space[0],3,FORECAST_LEN,out[0],3,FORECAST_LEN,count-PLOTFIT_LEN ,&Isfull);

		

            for(j = 0; j < FORECAST_LEN; j++)
            {
                out_predict[j].x = out[0][j];
                out_predict[j].y = out[1][j];
                out_predict[j].time = (*Z).time + T*(j+1);
                out_predict[j].pro = out[2][j];
								
								//printf("out = %f\r\n",out[0][j]);
            }
        *flag = 1;



}



void resetKalman()
{
	times_of_pos = 0;
	count = 0;
}





