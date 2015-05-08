#include "main.h"

float Globaldis;
void vPathPlanTask(void *pvParameters)
{
	int Result;
	char SendTrace[PACKET_LEN];
	float *pfloat,dis,tempAngle;
	int count = 0,total,end = 0;
	int tmpcount=1;
	Position TargetPos = CurPos;
	Position prePos = CurPos;
	SendTrace[0] = 0x00;
	SendTrace[1] = SELFADDRESS;
	SendTrace[2] = 0x03;
	SendTrace[3] = 0x46;
	vTaskDelay(3000);
//	total = generate_Pos(0,90,120,300);
#ifdef No1
//	setActiveArea(105,0,300,75);
	setActiveArea(0,0,90,300);
#endif

#ifdef No2
//	setActiveArea(0,0,75,195);
	setActiveArea(240,0,300,300);
#endif

#ifdef No3
//	setActiveArea(0,225,195,300);
	setActiveArea(240,0,300,300);
#endif

#ifdef No4
//	setActiveArea(225,105,300,300);
	setActiveArea(120,0,180,300);
#endif

#ifdef No5
//	setActiveArea(105,105,195,195);
	setActiveArea(240,0,300,300);
#endif

	Debug_printf("vPathPlanTask");
	GlobleCurProcedure.which_step = patrol;
	GloblePathProcedure.which_step = patrol;
	//No3 No4
	//prePos.Angle = 0;
	//
	while (1)
	{
		//stateTrans(preProcedure);
		//Result = AssignResult[OrderIndex[SELFADDRESS]];
		switch(GlobleCurProcedure.which_step)
		{
			case seenTarget: 
				Debug_printf("seenTarget\r\n");
				isFindOver = 0;
				tmpcount = 0;
			
				GloblePathProcedure.which_step =  seenTarget;
				TargetPos = GlobleCurProcedure.pvParameter.pos;
//				printf("Tarpos: %f %f %f\r\n",TargetPos.Angle,TargetPos.x,TargetPos.y);
				dis = sqrt(pow(TargetPos.y - CurPos.y, 2) + pow(TargetPos.x - CurPos.x, 2));
				Debug_printf("dis: %f\r\n", dis);
				if(dis < 25)
				{
//					printf(" I am in dis < 50 \r\n");
					TargetPos.x = TargetPos.x + 5 * arm_cos_f32((TargetPos.Angle / 180) * PI);
					TargetPos.y = TargetPos.y + 5 * arm_sin_f32((TargetPos.Angle / 180) * PI);
					Debug_printf("set end=1\r\n");
					end = 1;
				}
				else
				{
					Debug_printf(" I am in dis>25 \r\n");
					TargetPos.x = TargetPos.x - 0.9 * dis * arm_cos_f32((TargetPos.Angle / 180) * PI);
					TargetPos.y = TargetPos.y - 0.9 * dis * arm_sin_f32((TargetPos.Angle / 180) * PI);
				}
//				printf("Curpos: %f %f %f\r\n",CurPos.Angle,CurPos.x,CurPos.y);
//				printf("Tarpos: %f %f %f\r\n",TargetPos.Angle,TargetPos.x,TargetPos.y);
				break;
			case looseTarget:
				Debug_printf("looseTarget\r\n");
				GloblePathProcedure.which_step = looseTarget;
				TargetPos = searchTarg(tmpcount);
				tmpcount ++;
				if(tmpcount > 8)
				{
					isFindOver = 1;
					tmpcount = 0;
				}
//				printf("I am in looseTarget,tmpcount = %d \r\n",tmpcount);
				vTaskDelay(1500);
				break;
			case patrol:
				Debug_printf("patrol\r\n");
				GloblePathProcedure.which_step =  patrol;
				
#ifdef ENABLE_PATROL	

				TargetPos = getNextPos(prePos);
				prePos = TargetPos;
				
#endif
				
//				TargetPos.Angle = 90;
//				TargetPos.x = 0;
//				TargetPos.y = 180;
			
				
//					TargetPos.Angle = CurPos.Angle - 90;
//					if(TargetPos.Angle <= -180)
//						TargetPos.Angle += 360;
//				TargetPos.x = CurPos.x;
//					TargetPos.y = CurPos.y;
			
			
//				if(count >= total)
//				{
//					count = 0;
//				}
//				TargetPos = GoThroght[count++];
//					TargetPos = CurPos;
//					vTaskDelay(1000);
//				printf("end patrol\r\n");
//				printf("Curpos: %f %f %f\r\n",CurPos.Angle,CurPos.x,CurPos.y);
//				printf("Tarpos: %f %f %f\r\n",TargetPos.Angle,TargetPos.x,TargetPos.y);
				break;
			case goToOtherTar:
				GloblePathProcedure.which_step =  goToOtherTar;
				Debug_printf("goToOtherTar\r\n");
				
					
				//vTaskDelay(1000);
				//TargetPos.Angle = GlobleCurProcedure.pvParameter.pos.Angle;
				TargetPos = GlobleCurProcedure.pvParameter.pos;
				tempAngle = atan2(TargetPos.y - CurPos.y,TargetPos.x - CurPos.x)/PI*180;
				Debug_printf("Curpos: %f %f %f\r\n",CurPos.Angle,CurPos.x,CurPos.y);
				Debug_printf("BTarpos: %f %f %f\r\n",TargetPos.Angle,TargetPos.x,TargetPos.y);
				Debug_printf("tempAngle: %f\r\n",tempAngle);
				if( sqrt(pow(TargetPos.y - CurPos.y, 2) + pow(TargetPos.x - CurPos.x, 2)) > 60)
				{
					Debug_printf(">60\r\n");
					TargetPos.Angle = tempAngle;
					TargetPos.x = CurPos.x + 60 * arm_cos_f32((tempAngle / 180) * PI);
					TargetPos.y = CurPos.y + 60 * arm_sin_f32((tempAngle / 180) * PI);
				}
				
				
				Debug_printf("ATarpos: %f %f %f\r\n",TargetPos.Angle,TargetPos.x,TargetPos.y);
				break;
			default :
				break;
		}
		
//		printf("to in ifCollision\r\n");
		while (ifCollision(CurPos.x, CurPos.y, TargetPos.x, TargetPos.y))
		{
			Debug_printf("I am ifCollision\r\n");
			vTaskDelay(1000);
		}
		
		if(IsOutSide(TargetPos))
		{
			TargetPos = CurPos;
		}
		//printf("I am out ifCollision\r\n");
		/**********????????????*****************/
		pfloat = (float*)&SendTrace[LIS_START];
		pfloat[0] = CurPos.x;
		pfloat[1] = CurPos.y;
		pfloat[2] = TargetPos.x;
		pfloat[3] = TargetPos.y;
		xQueueSend(CC2500TxQueue, SendTrace, portMAX_DELAY);
		
//		printf("Curpos: %f %f %f\r\n",CurPos.Angle,CurPos.x,CurPos.y);
//		Debug_printf("Tarpos: %f %f %f\r\n",TargetPos.Angle,TargetPos.x,TargetPos.y);
//	  Debug_printf("CC2500TxQueue over\r\n");
//		xQueueSend(TargetPosQueue, &TargetPos, portMAX_DELAY);
//		Debug_printf("TargetPosQueue over\r\n");
		xSemaphoreTake(MoveComplete, portMAX_DELAY);
//		Debug_printf("MoveComplete over\r\n");
		//vTaskDelay(1000);
		if(end)
		{
			Debug_printf("end\r\n");
			while(1)
			{
				vTaskDelay(1000);
			}
		}
	}
}
