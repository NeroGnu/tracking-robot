#include "main.h"

void ProcessVideoResult(void);
void SerialHex(void);
void SerialTranslate(void);

void vBCRxGuardianTask(void *pvParameters)
{
	char TempInformation[TARGETINFORMATIONLENGTH] = {SELFADDRESS, 0};

//***********************************************************************
	char *pBuffer = NULL;
	FloatToChar *pfloat = NULL;
	int i, receivetime;
	int ChangeFlag = 0;
	float deltax, deltay;
	//buffer to vSeeTargetPosListMaintain
	char buffer[20 + 1];
//	int AriseFlag[TARGETNUM] = {0};
	char LastEfficiency[TARGETNUM + 1] = {SELFADDRESS, 0};
	unsigned int FrameCounter = 0;
	int UpdateCounter[TARGETNUM] = {0};
	
	FloatToChar *pfloatT, tempFTC;
	IntToChar *pint, tempITC;
	UIntToChar *puint, tempUITC;
	
	pfloatT = (FloatToChar*) &buffer[1];
	puint = (UIntToChar*) &pfloatT[3];
	pint = (IntToChar*) &pfloatT[4];
	
	
	pfloat = (FloatToChar*) &BCRxBuffer[RE_TARGETNUM];
//**************************************************************************

	DMA_Cmd(DMA1_Stream1,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
	
	/*********************************** Create timer ***********************************/
	xTimers = xTimerCreate
		          (  /* Just a text name, not used by the RTOS kernel. */
                     (const signed char*)"Timer",
                     /* The timer period in ticks. */
                     ( TAKE_DATA_TIME-(int)((Timer2Counter%T_TAKE_DATA_TIME) < (T_TAKE_DATA_TIME - Timer2Counter%T_TAKE_DATA_TIME) ? (Timer2Counter%T_TAKE_DATA_TIME) : (Timer2Counter%T_TAKE_DATA_TIME - T_TAKE_DATA_TIME))/10 ),
                     /* The timers will auto-reload themselves when they expire. */
                     pdTRUE,
                     /* Assign each timer a unique id equal to its array index. */
                     NULL,
                     /* Each timer calls the same callback when it expires. */
                     tTimerCallback
                   );

         if( xTimers == NULL )
         {
             /* The timer was not created. */
						 Debug_printf("timer created fail\r\n");
         }
         else
         {
             /* Start the timer.  No block time is specified, and even if one was
             it would be ignored because the RTOS scheduler has not yet been
             started. */
             if( xTimerStart( xTimers, 0 ) != pdPASS )
             {
							 Debug_printf("timer started fail\r\n");
                 /* The timer could not be set into the Active state. */
             }
         }
				 
				 
	for (;;)
	{
		if (pdTRUE == xSemaphoreTake(BCRxSyn, portMAX_DELAY))
		{
//****************************************			
//			SerialHex();
//			SerialTranslate();
//			printf("get!");
//			ProcessVideoResult();
//****************************************
			receivetime = Timer2Counter;
		
		  //update the global target efficiency.
			if (pdTRUE == xSemaphoreTake(SelfTargetInformationMutex, portMAX_DELAY))
			{
				arm_copy_q7(BCRxBuffer, SelfTargetInformation + 1, TARGETNUM);
				xSemaphoreGive(SelfTargetInformationMutex);
			}
			FrameCounter++;
			
			//check the change of target efficiency.
			for (i = 0; i < TARGETNUM; i++)
			{
				if (LastEfficiency[1 + i] != BCRxBuffer[i])
				{
					Debug_printf("LastEfficiency[%d]:%d, BCRxBuffer[%d]:%d\r\n", i, LastEfficiency[1 + i], i, BCRxBuffer[i]);
					ChangeFlag = 1;
					if (0 >= LastEfficiency[1 + i] && 0 < BCRxBuffer[i])
					{
						if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
						{
							USART2->SR;
							USART_SendData(UART4, '!');
							xSemaphoreGive(MoveStopEnable);
							Debug_printf("Force update current postion.\r\n");
							break;
						}
					}
//					break;
				}
			}
			
			//update the cache efficiency.
			arm_copy_q7(BCRxBuffer, LastEfficiency + 1, TARGETNUM);
			
			//update the cache time.
			Cache1Time = Cache2Time;
			Cache2Time = receivetime;
			for (i = 0; i < TARGETNUM; i++)
			{
				//update X coordinate.
				SelfXCoordinate[i] = pfloat[i];
				//update the target distance, which compute by X width.
				SelfXDistance[i].c[0] = pfloat[RE_TARGETNUM + i].c[0];
				SelfXDistance[i].c[1] = pfloat[RE_TARGETNUM + i].c[1];
				SelfXDistance[i].c[2] = pfloat[RE_TARGETNUM + i].c[2];
				SelfXDistance[i].c[3] = pfloat[RE_TARGETNUM + i].c[3];
				//flush cache1.
				Cache1[i] = Cache2[i];
				
				if (0 < (signed char)BCRxBuffer[i] || SelfXDistance[i].f > 0.001)
				{
					//If the the distance changed.
					Globaldis = SelfXDistance[i].f;
					//compute angle.
					TargetPositionList[i].Angle = CurPos.Angle + (float) computeAngle(SelfXDistance[i].f , 240 - SelfXCoordinate[i].f);
					//compute X, Y coordinate.
					deltax = SelfXDistance[i].f * arm_cos_f32((TargetPositionList[i].Angle / 180) * PI);
					deltay = SelfXDistance[i].f * arm_sin_f32((TargetPositionList[i].Angle / 180) * PI);
					TargetPositionList[i].x = CurPos.x + deltax;
					TargetPositionList[i].y = CurPos.y + deltay;
					//flush cache2.
					Cache2[i] = TargetPositionList[i];
					buffer[0] = i;
					
					//copy angle.
					tempFTC.f = TargetPositionList[i].Angle;
					pfloatT[0].c[0] = tempFTC.c[0];
					pfloatT[0].c[1] = tempFTC.c[1];
					pfloatT[0].c[2] = tempFTC.c[2];
					pfloatT[0].c[3] = tempFTC.c[3];
		      //copy X coordinate, which 30cm before target.
					tempFTC.f = CurPos.x + deltax - 30 * arm_cos_f32((TargetPositionList[i].Angle / 180) * PI);
					pfloatT[1].c[0] = tempFTC.c[0];
					pfloatT[1].c[1] = tempFTC.c[1];
					pfloatT[1].c[2] = tempFTC.c[2];
					pfloatT[1].c[3] = tempFTC.c[3];
		      //copy Y coordinate, which 30cm before target.
					tempFTC.f = CurPos.y + deltay - 30 * arm_sin_f32((TargetPositionList[i].Angle / 180) * PI);
					pfloatT[2].c[0] = tempFTC.c[0];
					pfloatT[2].c[1] = tempFTC.c[1];
					pfloatT[2].c[2] = tempFTC.c[2];
					pfloatT[2].c[3] = tempFTC.c[3];
		      //add timestamp.
					tempUITC.i = receivetime;
					puint[0].c[0] = tempUITC.c[0];
					puint[0].c[1] = tempUITC.c[1];
					puint[0].c[2] = tempUITC.c[2];
					puint[0].c[3] = tempUITC.c[3];
		      //mark the information come from self.
					tempITC.i = 1;
					pint[0].c[0] = tempITC.c[0];
					pint[0].c[1] = tempITC.c[1];
					pint[0].c[2] = tempITC.c[2];
					pint[0].c[3] = tempITC.c[3];
		      //send to vSeeTargetPosListMaintain.
					xQueueSend(TargetATPosQueue, buffer, portMAX_DELAY);
		
				}
				else
				{
					
					UpdateCounter[i]++;
					if (10 < UpdateCounter[i])
					{
						UpdateCounter[i] = 0;
						TargetPositionList[i].Angle = 0;
						TargetPositionList[i].x = -1.0;
						TargetPositionList[i].y = -1.0;
					}
		
				}
			}
			
			
			if (1 == ChangeFlag || FrameCounter > 10)
			{
				ChangeFlag = 0;
				FrameCounter = 0;
		
				if (pdTRUE == xSemaphoreTake(TargetInformationEmpty, portMAX_DELAY))
				{
					if (pdTRUE == xSemaphoreTake(TargetInformationCounterMutex, portMAX_DELAY))
					{				
						if (pBuffer = GetBuffer(&TargetInformationPool))
						{
							pBuffer[0] = SELFADDRESS;
							arm_copy_q7(BCRxBuffer, pBuffer + 1, TARGETNUM);
							xSemaphoreGive(TargetInformationCounterMutex);
							
						}
						xSemaphoreGive(TargetInformationFull);
					}
				}
			}
			
//				xSemaphoreGive(SelfXCoordinateSyn);
			
//			printf("out ProcessVideoResult\r\n");
//			printf("%d\r\n", AccuracyTest());
		}
	}
}


void ProcessVideoResult()
{
	char *pBuffer = NULL;
	FloatToChar *pfloat = NULL;
	int i, receivetime;
	int ChangeFlag = 0;
	float deltax, deltay;
	//TargetATPos *pTATP;
	char buffer[20 + 1];
	static char LastEfficiency[TARGETNUM + 1] = {SELFADDRESS, 0};
	static unsigned int FrameCounter = 0;
	static int UpdateCounter[TARGETNUM] = {0};
	
	FloatToChar *pfloatT, tempFTC;
	IntToChar *pint, tempITC;
	UIntToChar *puint, tempUITC;
	
	pfloatT = (FloatToChar*) &buffer[1];
	puint = (UIntToChar*) &pfloatT[3];
	pint = (IntToChar*) &pfloatT[4];
	
//	pTATP = (TargetATPos*) &buffer[1];
	
	pfloat = (FloatToChar*) &BCRxBuffer[RE_TARGETNUM];
	
	receivetime = Timer2Counter;
	
	
	//printf("0\r\n");
	if (pdTRUE == xSemaphoreTake(SelfTargetInformationMutex, portMAX_DELAY))
	{
		arm_copy_q7(BCRxBuffer, SelfTargetInformation + 1, TARGETNUM);
		xSemaphoreGive(SelfTargetInformationMutex);
	}
	FrameCounter++;
	
	for (i = 0; i < TARGETNUM; i++)
	{
		if (LastEfficiency[1 + i] != BCRxBuffer[1 + i])
		{
			ChangeFlag = 1;
			break;
		}
	}
	
	arm_copy_q7(BCRxBuffer, LastEfficiency + 1, TARGETNUM);
	
	//printf("1\r\n");
	//printf("SelfTargetInformation: ");
	Cache1Time = Cache2Time;
	Cache2Time = receivetime;
	for (i = 0; i < TARGETNUM; i++)
	{
//		printf("SelfTargetInformation[%d]:%d \r\n",i,SelfTargetInformation[i + 1]);
		SelfXCoordinate[i] = pfloat[i];
		SelfXDistance[i].c[0] = pfloat[RE_TARGETNUM + i].c[0];
		SelfXDistance[i].c[1] = pfloat[RE_TARGETNUM + i].c[1];
		SelfXDistance[i].c[2] = pfloat[RE_TARGETNUM + i].c[2];
		SelfXDistance[i].c[3] = pfloat[RE_TARGETNUM + i].c[3];
		Cache1[i] = Cache2[i];
		if (0 < (signed char)BCRxBuffer[i] || SelfXDistance[i].f > 0.001)
		{
			//printf("2\r\n");
//			printf("SelfXDistance :%f \r\n",SelfXDistance[i].f);
			Globaldis = SelfXDistance[i].f;
			TargetPositionList[i].Angle = CurPos.Angle + (float) computeAngle(SelfXDistance[i].f , 240 - SelfXCoordinate[i].f);
			deltax = SelfXDistance[i].f * arm_cos_f32((TargetPositionList[i].Angle / 180) * PI);
			deltay = SelfXDistance[i].f * arm_sin_f32((TargetPositionList[i].Angle / 180) * PI);
			TargetPositionList[i].x = CurPos.x + deltax;
			TargetPositionList[i].y = CurPos.y + deltay;
			Cache2[i] = TargetPositionList[i];
			//printf("2\r\n");
			buffer[0] = i;
			//printf("2\r\n");
//			printf("TargetPositionList[i].Angle: %f %f %f\r\n", TargetPositionList[i].Angle, TargetPositionList[i].x, TargetPositionList[i].y);
//			printf("SeePos: ");
			tempFTC.f = TargetPositionList[i].Angle;
			pfloatT[0].c[0] = tempFTC.c[0];
			pfloatT[0].c[1] = tempFTC.c[1];
			pfloatT[0].c[2] = tempFTC.c[2];
			pfloatT[0].c[3] = tempFTC.c[3];
//			printf("%f ", tempFTC.f);
			tempFTC.f = CurPos.x + deltax - 30 * arm_cos_f32((TargetPositionList[i].Angle / 180) * PI);
			pfloatT[1].c[0] = tempFTC.c[0];
			pfloatT[1].c[1] = tempFTC.c[1];
			pfloatT[1].c[2] = tempFTC.c[2];
			pfloatT[1].c[3] = tempFTC.c[3];
//			printf("%f ", tempFTC.f);
			tempFTC.f = CurPos.y + deltay - 30 * arm_sin_f32((TargetPositionList[i].Angle / 180) * PI);
			pfloatT[2].c[0] = tempFTC.c[0];
			pfloatT[2].c[1] = tempFTC.c[1];
			pfloatT[2].c[2] = tempFTC.c[2];
			pfloatT[2].c[3] = tempFTC.c[3];
//			printf("%f ", tempFTC.f);
			tempUITC.i = receivetime;
			puint[0].c[0] = tempUITC.c[0];
			puint[0].c[1] = tempUITC.c[1];
			puint[0].c[2] = tempUITC.c[2];
			puint[0].c[3] = tempUITC.c[3];
//			printf("%d ", tempUITC.i);
			tempITC.i = 1;
			pint[0].c[0] = tempITC.c[0];
			pint[0].c[1] = tempITC.c[1];
			pint[0].c[2] = tempITC.c[2];
			pint[0].c[3] = tempITC.c[3];
//			printf("%d\r\n", tempITC.i);
//			printf("before TargetATPosQueue\r\n");
			xQueueSend(TargetATPosQueue, buffer, portMAX_DELAY);
//			printf("after TargetATPosQueue\r\n");
		}
		else
		{
			UpdateCounter[i]++;
			if (10 < UpdateCounter[i])
			{
				UpdateCounter[i] = 0;
   			TargetPositionList[i].Angle = 0;
  			TargetPositionList[i].x = -1.0;
  			TargetPositionList[i].y = -1.0;
			}
//			TargetPositionList[i].Angle = 0;
//			TargetPositionList[i].x = -1.0;
//			TargetPositionList[i].y = -1.0;
		}
	}
	//printf("\r\n");
	
	if (1 == ChangeFlag || FrameCounter > 10)
	{
		ChangeFlag = 0;
		FrameCounter = 0;
//		printf("wake decision\r\n");
		if (pdTRUE == xSemaphoreTake(TargetInformationEmpty, portMAX_DELAY))
		{
			if (pdTRUE == xSemaphoreTake(TargetInformationCounterMutex, portMAX_DELAY))
			{				
				if (pBuffer = GetBuffer(&TargetInformationPool))
				{
					pBuffer[0] = SELFADDRESS;
					arm_copy_q7(BCRxBuffer, pBuffer + 1, TARGETNUM);
					xSemaphoreGive(TargetInformationCounterMutex);
					
				}
				xSemaphoreGive(TargetInformationFull);
			}
		}
	}
	
		xSemaphoreGive(SelfXCoordinateSyn);
} 


void SerialHex()
{
	int i;
	Debug_printf("Data\r\n");
	for (i = 0; i < BOARDRXLEN; i++)
	{
		Debug_printf("%d, ", BCRxBuffer[i]);
	}
}

void SerialTranslate(void)
{
	int i;
	FloatToChar temp;
	float *pfloat = NULL;
	pfloat = (float*) &BCRxBuffer[6];
	printf("Data\r\n");
	for (i = 0; i < 6; i++)
	{
		printf("%d, ", BCRxBuffer[i]);
	}
	printf("\r\n");
	
	for (i = 0; i < 6; i++)
	{
		temp.c[0] = BCRxBuffer[i*4+6+0];
		temp.c[1] = BCRxBuffer[i*4+6+1];
		temp.c[2] = BCRxBuffer[i*4+6+2];
		temp.c[3] = BCRxBuffer[i*4+6+3];
		
		printf("%f, ", temp.f);
	}
	printf("\r\n");
	
	for (i = 0; i < 6; i++)
	{
		temp.c[0] = BCRxBuffer[i*4+30+0];
		temp.c[1] = BCRxBuffer[i*4+30+1];
		temp.c[2] = BCRxBuffer[i*4+30+2];
		temp.c[3] = BCRxBuffer[i*4+30+3];
		
		printf("%f, ", temp.f);
	}
	printf("\r\n");
}

void ConvertionByWeight(char *src, char *result)
{
	int i;
	float temp1[TARGETNUM] = {0};
  float temp2[TARGETNUM] = {0};
	float *psrc = (float*) src;
	
	arm_offset_f32(psrc, -240, temp1, TARGETNUM);
	arm_abs_f32(temp1, temp1, TARGETNUM);
	arm_scale_f32(temp1, (1000000.0/240.0)*0.7, temp1, TARGETNUM);
	
	arm_mult_f32(psrc + 2*TARGETNUM, psrc + 3*TARGETNUM, temp2, TARGETNUM);
	arm_scale_f32(temp2, (1000000.0/(480*272))*0.3, temp2, TARGETNUM);
	
	arm_add_f32(temp1, temp2, temp1, TARGETNUM);
	arm_scale_f32(temp1, 127.0/1000000.0, temp1, TARGETNUM);
	
	for (i = 0; i < TARGETNUM; i++)
	{
		result[i] = (char) temp1[i];
	}
}
