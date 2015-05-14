
#ifndef VTRACEFORECASTTASK_H
#define VTRACEFORECASTTASK_H

typedef struct{
	float angle;
	float x;
	float y;
	int time;
}time_pos;

typedef struct{
    float x;
	float y;
	int time;
	float pro;
}Pred;

int Line_kalman(float *pre_Xkf,						//???????? (??)4?1?
				float *pre_P,						//???????? (??) 4?4?
				float *now_Z,							//????????  (??) 2?1?
				float *pre_R,						//?????????(??) 	2?2?
				float *now_Xkf,									// ????????  (??) 4?1?
				float *now_R,									//?????????(??) 2?2?
				float *now_P,									//????????(??)  4?4?
				time_pos *predict, int len_PD					//?????????(??)
				);

#define ENDURE_DIS  		30
#define FORECAST_LEN      	10
#define ENDURE_TIME 		7
#define PLOTFIT_LEN			6
#define STEP				3
#define T 1
#define N 60/T


int Jugemotor(float *estimate_value,int m1,int n1,
			  float observe_x,float observe_y,
			  int *times,
			  int *Up_down,
			  float *p_x,float *p_y
			);
int confirm(float *space, int m1,int n1,
			float observe_x,float observe_y,
			int num,int select,
			float *pre_confirm_times);
int forecast(float *px,float *py,
			float p,
			int start,
			float confirm_times,
			float *out,int m2,int n2);




#endif
