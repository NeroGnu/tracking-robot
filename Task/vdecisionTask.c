#include "main.h"

//int tempRes;

int Result;
Position alreadySeenPos,alreadyOtherPos;
void handleResNotNeg(int Result,Procedure toPro)
{
	Position tmpPos;
//	printf("SelfTargetInformation[Result+1]: %d\r\n",SelfTargetInformation[Result+1]);
	if( SelfTargetInformation[Result+1] != 0)
	{
		
//		tempRes = Result;
		
		if(GlobleCurProcedure.which_step != seenTarget)
		{
			printf("I see you\r\n");
			
			if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
			{
				printf("needStop\r\n");
				USART2->SR;
				USART_SendData(UART4, '!');
				xSemaphoreGive(MoveStopEnable);
				
				xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
			}
//			vTaskDelay(100);
		}
		
		GlobleCurProcedure.which_step = seenTarget; 
		tmpPos.Angle = CurPos.Angle + (float) computeAngle(SelfXDistance[Result].f , 240 - SelfXCoordinate[Result].f);
		tmpPos.x = CurPos.x + SelfXDistance[Result].f * arm_cos_f32((TargetPositionList[Result].Angle / 180) * PI);
		tmpPos.y = CurPos.y + SelfXDistance[Result].f * arm_sin_f32((TargetPositionList[Result].Angle / 180) * PI);
		GlobleCurProcedure.pvParameter.pos = tmpPos;
		alreadySeenPos = tmpPos;
//		printf("TargetPositionList[%d]: %f %f %f\r\n",Result,TargetPositionList[Result].Angle,TargetPositionList[Result].x,TargetPositionList[Result].y);
//		printf("tmpPos: %f %f %f\r\n",tmpPos.Angle,tmpPos.x,tmpPos.y);
		while(GloblePathProcedure.which_step != seenTarget)
		{
			vTaskDelay(10);
		}
	}
	else
	{
		printf("I can't see\r\n");
		if (SeeTargetPosList[Result].x >= 0 || SeeTargetPosList[Result].y >= 0)
		{
			if(TargetPositionList[Result].x >= 0 || TargetPositionList[Result].y >= 0)
			{
				printf("update1\r\n");
//				tempRes = Result;
				if(GlobleCurProcedure.which_step != seenTarget )
				{
					printf("I see you\r\n");
					
					if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
					{
						printf("needStop\r\n");
						USART2->SR;
						USART_SendData(UART4, '!');
						xSemaphoreGive(MoveStopEnable);
						
						xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
					}
		//			vTaskDelay(100);
				}
				GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = TargetPositionList[Result];
				//printf("TargetPositionList[%d]: %f %f %f\r\n",Result,TargetPositionList[Result].Angle,TargetPositionList[Result].x,TargetPositionList[Result].y);

			}
			else
			{
				printf("update0\r\n");
				if(GlobleCurProcedure.which_step != goToOtherTar && GlobleCurProcedure.which_step != seenTarget)
				{
					
					if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
					{
						printf("needStop\r\n");
						USART2->SR;
						USART_SendData(UART4, '!');
						xSemaphoreGive(MoveStopEnable);
						
						xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
					}
				}
				GlobleCurProcedure.which_step = goToOtherTar;
				GlobleCurProcedure.pvParameter.pos.Angle =  SeeTargetPosList[Result].Angle;
				GlobleCurProcedure.pvParameter.pos.x = SeeTargetPosList[Result].x;
				GlobleCurProcedure.pvParameter.pos.y = SeeTargetPosList[Result].y;
				alreadyOtherPos = GlobleCurProcedure.pvParameter.pos;
//				printf("SeeTargetPosList[%d]: %f %f %f\r\n",Result,SeeTargetPosList[Result].Angle,SeeTargetPosList[Result].x,SeeTargetPosList[Result].y);
				while(GloblePathProcedure.which_step != goToOtherTar)
				{
					printf("die\r\n");
					vTaskDelay(10);
				}
			}
			
		}
		else
		{
			if(GlobleCurProcedure.which_step == looseTarget)				//专门对于looseTarget
			{
				if(isFindOver == 1)
					GlobleCurProcedure.which_step = patrol;
				else
					GlobleCurProcedure.which_step = looseTarget;
			}
			else
				GlobleCurProcedure.which_step = toPro.which_step;
		}
	}
}

void vDecisionTask(void *pvParameters)
{
	int countLoosTimes = 0,found = 0;
	Procedure preProcedure,tmpPro,pre_pro;
	Position temPos;
	preProcedure.which_step = patrol;
	pre_pro.which_step = patrol;
	while(1)
	{
		if (pdTRUE == xSemaphoreTake(ComunicationWithBoardSyn, portMAX_DELAY))
		{
			Result = AssignResult[OrderIndex[SELFADDRESS]];
			printf("Result: %d\r\n",Result);
//			printf("SelfTargetInformation[1]: %d\r\n",SelfTargetInformation[1]);
			switch(preProcedure.which_step)
			{
				case seenTarget: 
					if(-1 == Result)
					{
						countLoosTimes++;
						if(countLoosTimes > 1)
						{
							countLoosTimes = 0; 
							GlobleCurProcedure.which_step = looseTarget;
						}
						else
						{
							GlobleCurProcedure.which_step = seenTarget;
							GlobleCurProcedure.pvParameter.pos = alreadySeenPos;
						}
						
						
//						GlobleCurProcedure.which_step = looseTarget;
//						GlobleCurProcedure.which_step = patrol;
//						
//						countLoosTimes++;
//						if(countLoosTimes > 2)
//						{
//							countLoosTimes = 0; 
//							GlobleCurProcedure.which_step = patrol;
//						}
//						else
//						{
//							//vTaskDelay(10);
////							GlobleCurProcedure.which_step = seenTarget;
//							GlobleCurProcedure.pvParameter.pos = TargetPositionList[tempRes];
//						}
					}
					else
					{
						tmpPro.which_step = looseTarget;
						handleResNotNeg(Result,tmpPro);
//						if( SelfTargetInformation[Result+1] != 0)
//						{
//							tempRes = Result;
//							GlobleCurProcedure.which_step = seenTarget; 
//							GlobleCurProcedure.pvParameter.pos = TargetPositionList[Result];
//						}
//						else
//						{
//							if (SeeTargetPosList[Result].x > 0 || SeeTargetPosList[Result].y > 0)
//							{
//								GlobleCurProcedure.which_step = goToOtherTar;
//								GlobleCurProcedure.pvParameter.pos.Angle =  SeeTargetPosList[Result].Angle;
//								GlobleCurProcedure.pvParameter.pos.x = SeeTargetPosList[Result].x;
//								GlobleCurProcedure.pvParameter.pos.y = SeeTargetPosList[Result].y;
//								printf("SeeTargetPosList[%d]: %f %f %f\r\n",Result,SeeTargetPosList[Result].Angle,SeeTargetPosList[Result].x,SeeTargetPosList[Result].y);
//							}
//							else
//							{
//								GlobleCurProcedure.which_step = looseTarget;
//							}
//						}						
					}
					break;
				case looseTarget:
					if(-1 == Result )
					{
						if(isFindOver == 1)
							GlobleCurProcedure.which_step = patrol;
						else
							GlobleCurProcedure.which_step = looseTarget;
						//GlobleCurProcedure.which_step = patrol;
					}
					else
					{
////						printf("Result: %d\r\n",Result);
//						countLoosTimes = 0;
//						tempRes = Result;
						tmpPro.which_step = patrol;
//						if(pre_pro.which_step == preProcedure.which_step)
//							handleResNotNeg(Result,0,tmpPro);
//						else
							handleResNotNeg(Result,tmpPro);
						
						
						
//						
//						if( SelfTargetInformation[Result+1] != 0)
//						{
//							GlobleCurProcedure.which_step = seenTarget;
//							GlobleCurProcedure.pvParameter.pos = TargetPositionList[Result];
////							printf("TargetPositionList[%d]: %f %f %f\r\n",Result,TargetPositionList[Result].Angle,TargetPositionList[Result].x,TargetPositionList[Result].y);
//						
//	//						if (pdTRUE == xSemaphoreTake(MoveStopEnable, portMAX_DELAY))
//	//						{
//	//							USART2->SR;
//	//							USART_SendData(UART4, '!');
//	//							xSemaphoreGive(MoveStopEnable);
//	//							
//	//							//xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
//	//						}
//	//						vTaskDelay(100);
//						}
//						else 
//						{
//							if (SeeTargetPosList[Result].x > 0 || SeeTargetPosList[Result].y > 0)
//							{
//								GlobleCurProcedure.which_step = goToOtherTar;
//								GlobleCurProcedure.pvParameter.pos.Angle =  SeeTargetPosList[Result].Angle;
//								GlobleCurProcedure.pvParameter.pos.x = SeeTargetPosList[Result].x;
//								GlobleCurProcedure.pvParameter.pos.y = SeeTargetPosList[Result].y;
////								printf("SeeTargetPosList[%d]: %f %f %f\r\n",Result,SeeTargetPosList[Result].Angle,SeeTargetPosList[Result].x,SeeTargetPosList[Result].y);
//							}
//							else
//							{
//								if(isFindOver == 1)
//									GlobleCurProcedure.which_step = patrol;
//								else
//									GlobleCurProcedure.which_step = looseTarget;
//							}
//						}
					}
					break;
				case patrol:
					if(-1 == Result)
					{
						//printf("i am in res = -1\r\n");
						GlobleCurProcedure.which_step = patrol;
					}
					else
					{
//						printf("Result: %d\r\n",Result);
						countLoosTimes = 0;
//						tempRes = Result;
						tmpPro.which_step = patrol;
						handleResNotNeg(Result,tmpPro);
						//handleResNotNeg(Result,1,tmpPro);
//						if( SelfTargetInformation[Result+1] != 0)
//						{
//							GlobleCurProcedure.which_step = seenTarget;
//							GlobleCurProcedure.pvParameter.pos = TargetPositionList[Result];
//							printf("TargetPositionList[%d]: %f %f %f\r\n",Result,TargetPositionList[Result].Angle,TargetPositionList[Result].x,TargetPositionList[Result].y);
//						
//	//						if (pdTRUE == xSemaphoreTake(MoveStopEnable, portMAX_DELAY))
//	//						{
//	//							USART2->SR;
//	//							USART_SendData(UART4, '!');
//	//							xSemaphoreGive(MoveStopEnable);
//	//							
//	//							//xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
//	//						}
//	//						vTaskDelay(100);
//						}
//						else 
//						{
//							if (SeeTargetPosList[Result].x > 0 || SeeTargetPosList[Result].y > 0)
//							{
//								GlobleCurProcedure.which_step = goToOtherTar;
//								GlobleCurProcedure.pvParameter.pos.Angle =  SeeTargetPosList[Result].Angle;
//								GlobleCurProcedure.pvParameter.pos.x = SeeTargetPosList[Result].x;
//								GlobleCurProcedure.pvParameter.pos.y = SeeTargetPosList[Result].y;
//								printf("SeeTargetPosList[%d]: %f %f %f\r\n",Result,SeeTargetPosList[Result].Angle,SeeTargetPosList[Result].x,SeeTargetPosList[Result].y);
//							}
//							else
//							{
//									GlobleCurProcedure.which_step = patrol;
//							}
//						}
					}
					break;
				case goToOtherTar:
					if(-1 == Result)
					{
						countLoosTimes++;
						if(countLoosTimes > 1)
						{
							countLoosTimes = 0; 
							GlobleCurProcedure.which_step = looseTarget;
						}
						else
						{
							GlobleCurProcedure.which_step = goToOtherTar;
							GlobleCurProcedure.pvParameter.pos = alreadyOtherPos;
						}
//						GlobleCurProcedure.which_step = looseTarget;
//						countLoosTimes++;
//						if(countLoosTimes > 20)
//						{
//							countLoosTimes = 0; 
//							GlobleCurProcedure.which_step = looseTarget;
//						}
					}
					else
					{
						tmpPro.which_step = patrol;
//						if(pre_pro.which_step == preProcedure.which_step)
//							handleResNotNeg(Result,0,tmpPro);
//						else
							handleResNotNeg(Result,tmpPro);
//						if( SelfTargetInformation[Result+1] != 0)
//						{
//							GlobleCurProcedure.which_step = seenTarget; 
//							GlobleCurProcedure.pvParameter.pos = TargetPositionList[Result];
//						}
//						else
//						{
//							if (SeeTargetPosList[Result].x > 0 || SeeTargetPosList[Result].y > 0)
//							{
//								GlobleCurProcedure.which_step = goToOtherTar;
//								GlobleCurProcedure.pvParameter.pos.Angle =  SeeTargetPosList[Result].Angle;
//								GlobleCurProcedure.pvParameter.pos.x = SeeTargetPosList[Result].x;
//								GlobleCurProcedure.pvParameter.pos.y = SeeTargetPosList[Result].y;
//								printf("SeeTargetPosList[%d]: %f %f %f\r\n",Result,SeeTargetPosList[Result].Angle,SeeTargetPosList[Result].x,SeeTargetPosList[Result].y);
//							}
//							else
//							{
//									GlobleCurProcedure.which_step = patrol;
//							}
//						}
					}
					break;
				default :
					break;
			}
			pre_pro = preProcedure;
			preProcedure = GloblePathProcedure;
		}
	}
}