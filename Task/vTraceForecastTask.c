#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "main.h"






//Position t = {40.0,50.0,60.0};

int HaveData = 0;
float SelfXDistance_f;
Position SeenPos;
void vTraceForecastTask(void *pvParameters) 
{


	int totol_num = 0;
	int MyHaveData = 0;
	
	time_pos Observ;

	Pred out_predict[FORECAST_LEN];

	int flag = 0,j,preFlag = 0;

	for(;;)	
	{
		xSemaphoreTake(TimerxSyn, portMAX_DELAY);
		//time_cache = time_cache + 1;
		
		
		
		
		Result = AssignResult[OrderIndex[SELFADDRESS]];
		
//		Debug_printf("Result: %d\r\n",Result);
		
		SelfXDistance_f = SelfXDistance[Result].f;
		/*Determine whether there is data passed down*/
		if(Result >= 0)
		{
			now_res = Result;
//			if(SeeTargetPosList[now_res].update == 1)
			if(TargetPositionList[now_res].x != -1 || TargetPositionList[now_res].y != -1 )
			{
			MyHaveData = 1;
//			Observ.angle = CurPos.Angle + (float) computeAngle(SelfXDistance[Result].f , 240 - SelfXCoordinate[Result].f);
//			Observ.x = CurPos.x + SelfXDistance[Result].f * arm_cos_f32((TargetPositionList[Result].Angle / 180) * PI);
//			Observ.y = CurPos.y + SelfXDistance[Result].f * arm_sin_f32((TargetPositionList[Result].Angle / 180) * PI);
			Observ.angle = TargetPositionList[Result].Angle;
			Observ.x = TargetPositionList[Result].x;
			Observ.y = TargetPositionList[Result].y;
			if(abs(T_TIMER - Cache1Time) > abs(T_TIMER - Cache2Time))
			{
				Observ.time = Cache2Time;
			}
			else
			{
				Observ.time = Cache1Time;
			}
			
			
			
//			if(Cache1[Result].x >= (float)(0.03) ||  Cache1[Result].y >= (float)(0.03) || Cache2[Result].x >= (float)(0.03) ||  Cache2[Result].y >= (float)(0.03))
//			{
//				MyHaveData = 1;
//				Debug_printf("have data\r\n");
//				if(abs(T_TIMER - Cache1Time) > abs(T_TIMER - Cache2Time))
//				{
//					Observ.x = Cache2[Result].x;
//					Observ.y = Cache2[Result].y;
//					Observ.angle = Cache2[Result].Angle;
//					Observ.time = Cache2Time;
//				}
//				else
//				{
//					Observ.x = Cache1[Result].x;
//					Observ.y = Cache1[Result].y;
//					Observ.angle = Cache1[Result].Angle;
//					Observ.time = Cache1Time;
//				}
			}
			else
			{
				MyHaveData = 0;
//				Debug_printf("no data\r\n");
			}
		}
			else
			{
				MyHaveData = 0;
//				Debug_printf("no data\r\n");
			}
		
		

		/*Do part of the decision*/
		if(MyHaveData == 0 && preFlag == 0)//没数据也没启动滤波
		{
//			Debug_printf("0 0\r\n");
			HaveData = 0;
			totol_num = 0;
			//GloblePathProcedure.which_step =  patrol;
		}
		else if(MyHaveData == 1 && preFlag == 0)//上面传过来数据，但还没进入滤波
		{
			HaveData = 1;
			SeenPos.Angle = Observ.angle;
			SeenPos.x = Observ.x;
			SeenPos.y = Observ.y;
			Debug_printf("SeenPos: %f %f %f\r\n",SeenPos.Angle,SeenPos.x,SeenPos.y);
			Debug_printf("1 0\r\n");
			if(GlobleCurProcedure.which_step != seenTarget  && end == 0)
			{
				Debug_printf("I see you\r\n");
				
				GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = SeenPos;
				
				if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
				{
					Debug_printf("needStop\r\n");
					USART2->SR;
					USART_SendData(UART4, '!');
					xSemaphoreGive(MoveStopEnable);
					
					xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
				}
				
	//			vTaskDelay(100);
			}
//			SeenPos.Angle = CurPos.Angle + (float) computeAngle(SelfXDistance[Result].f , 240 - SelfXCoordinate[Result].f);
//			SeenPos.x = CurPos.x + SelfXDistance_f * arm_cos_f32((TargetPositionList[Result].Angle / 180) * PI);
//			SeenPos.y = CurPos.y + SelfXDistance_f * arm_sin_f32((TargetPositionList[Result].Angle / 180) * PI);

				SeenPos.Angle = Observ.angle;
				SeenPos.x = Observ.x;
				SeenPos.y = Observ.y;


			GlobleCurProcedure.which_step = seenTarget;
			GlobleCurProcedure.pvParameter.pos = SeenPos;
			Debug_printf("GlobleCurProcedure: %f %f %f\r\n",GlobleCurProcedure.pvParameter.pos.Angle,GlobleCurProcedure.pvParameter.pos.x,GlobleCurProcedure.pvParameter.pos.y);
			
			totol_num = 0;
			State_prediction(&Observ,out_predict,&flag);
			preFlag = flag;
		}
		else if(MyHaveData == 0 && preFlag == 1)   //上面没有数据来，但已经进入kalman滤波
		{
			Debug_printf("0 1\r\n");
			printf("SeenPos: %f %f %f\r\n",SeenPos.Angle,SeenPos.x,SeenPos.y);
			Observ.x = out_predict[0].x;										//似乎有问题
			Observ.y = out_predict[0].y;
			Observ.time = out_predict[0].time;
			
			
			HaveData = 1;
			SeenPos.Angle = Observ.angle;
			SeenPos.x = Observ.x;
			SeenPos.y = Observ.y;
			
			if(GlobleCurProcedure.which_step != seenTarget  && end == 0)
			{
				Debug_printf("I see you\r\n");
				
				GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = SeenPos;
			
				if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
				{
					Debug_printf("needStop\r\n");
					USART2->SR;
					USART_SendData(UART4, '!');
					xSemaphoreGive(MoveStopEnable);
					
					xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
				}
	//			vTaskDelay(100);
			}
//			SeenPos.Angle = CurPos.Angle + (float) computeAngle(SelfXDistance[Result].f , 240 - SelfXCoordinate[Result].f);
//			SeenPos.x = CurPos.x + SelfXDistance_f * arm_cos_f32((TargetPositionList[Result].Angle / 180) * PI);
//			SeenPos.y = CurPos.y + SelfXDistance_f * arm_sin_f32((TargetPositionList[Result].Angle / 180) * PI);
				SeenPos.Angle = Observ.angle;
				SeenPos.x = Observ.x;
				SeenPos.y = Observ.y;
			GlobleCurProcedure.which_step = seenTarget;
			GlobleCurProcedure.pvParameter.pos = SeenPos;
			
			totol_num++;
			if(totol_num >= FORECAST_LEN)
			{
				Debug_printf("reset\r\n");
				HaveData = 0;
				preFlag = 0;
				totol_num = 0;
				GlobleCurProcedure.which_step = looseTarget;
				
			}
			else
			{
				State_prediction(&Observ,out_predict,&flag);
				preFlag = flag;
			}
		}
		else																				//上面有数据来，已经进入kalman滤波
		{
			Debug_printf("1 1\r\n");
			HaveData = 1;
			SeenPos.Angle = Observ.angle;
			SeenPos.x = Observ.x;
			SeenPos.y = Observ.y;
			
			if(GlobleCurProcedure.which_step != seenTarget && end == 0)
			{
				Debug_printf("I see you\r\n");
				
				GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = SeenPos;
				
				if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
				{
					Debug_printf("needStop\r\n");
					USART2->SR;
					USART_SendData(UART4, '!');
					xSemaphoreGive(MoveStopEnable);
					
					xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
				}

	//			vTaskDelay(100);
			}
//			SeenPos.Angle = CurPos.Angle + (float) computeAngle(SelfXDistance[Result].f , 240 - SelfXCoordinate[Result].f);
//			SeenPos.x = CurPos.x + SelfXDistance_f * arm_cos_f32((TargetPositionList[Result].Angle / 180) * PI);
//			SeenPos.y = CurPos.y + SelfXDistance_f * arm_sin_f32((TargetPositionList[Result].Angle / 180) * PI);
			SeenPos.Angle = Observ.angle;
				SeenPos.x = Observ.x;
				SeenPos.y = Observ.y;
			GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = SeenPos;
			
			
			State_prediction(&Observ,out_predict,&flag);
			preFlag = flag;
		}
		
		xSemaphoreGive(ComunicationWithTraceSyn);
	}	
		

		
	
}



