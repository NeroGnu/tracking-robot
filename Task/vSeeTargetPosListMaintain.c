#include "main.h"

#define ERRORTHRESHOLD_A    (3)
#define ERRORTHRESHOLD_X    (5)
#define ERRORTHRESHOLD_Y    (5)

void vSeeTargetPosListMaintain(void *pvParameters)
{
	char buffer[20 + 1];
	char SendTargetATPos[PACKET_LEN];
	unsigned int SendTime[TARGETNUM] = {0}, LastCheck = 0;
	TargetATPos *pTATP;
	int i, changeFlag = 0;
	
	FloatToChar *pfloatT, tempFTC;
	IntToChar *pint, tempITC;
	UIntToChar *puint, tempUITC;
	
	pfloatT = (FloatToChar*) &buffer[1];
	puint = (UIntToChar*) &pfloatT[3];
	pint = (IntToChar*) &pfloatT[4];
	
	SendTargetATPos[DES_NUM] = 0x00;
	SendTargetATPos[SRC_NUM] = SELFADDRESS;
	SendTargetATPos[PRC_NUM] = 0x04;
	SendTargetATPos[LEN_NUM] = 1 + 20;
	
	for (i = 0; i < TARGETNUM; i++)
	{
		SeeTargetPosList[i].x = -1.0;
		SeeTargetPosList[i].y = -1.0;
		SeeTargetPosList[i].time = 0;
		SeeTargetPosList[i].update = 0;
	}
	
	for (;;)
	{
		if (pdTRUE == xQueueReceive(TargetATPosQueue, buffer, 1000))
		{
//			printf("RSeePos: ");
			tempFTC.c[0] = pfloatT[0].c[0];
			tempFTC.c[1] = pfloatT[0].c[1];
			tempFTC.c[2] = pfloatT[0].c[2];
			tempFTC.c[3] = pfloatT[0].c[3];
//			if (fabs(tempFTC.f - SeeTargetPosList[buffer[0]].Angle) > ERRORTHRESHOLD_A)
//			{
//				changeFlag = 1;
//			}
			SeeTargetPosList[buffer[0]].Angle = tempFTC.f;
//			printf("%f ", tempFTC.f);
			tempFTC.c[0] = pfloatT[1].c[0];
			tempFTC.c[1] = pfloatT[1].c[1];
			tempFTC.c[2] = pfloatT[1].c[2];
			tempFTC.c[3] = pfloatT[1].c[3];
//			if (fabs(tempFTC.f - SeeTargetPosList[buffer[0]].x) > ERRORTHRESHOLD_X)
//			{
//				changeFlag = 1;
//			}
			SeeTargetPosList[buffer[0]].x = tempFTC.f;
//			printf("%f ", tempFTC.f);
			tempFTC.c[0] = pfloatT[2].c[0];
			tempFTC.c[1] = pfloatT[2].c[1];
			tempFTC.c[2] = pfloatT[2].c[2];
			tempFTC.c[3] = pfloatT[2].c[3];
//			if (fabs(tempFTC.f - SeeTargetPosList[buffer[0]].y) > ERRORTHRESHOLD_Y)
//			{
//				changeFlag = 1;
//			}
			SeeTargetPosList[buffer[0]].y = tempFTC.f;
//			printf("%f ", tempFTC.f);
			tempUITC.c[0] = puint[0].c[0];
			tempUITC.c[1] = puint[0].c[1];
			tempUITC.c[2] = puint[0].c[2];
			tempUITC.c[3] = puint[0].c[3];
			SeeTargetPosList[buffer[0]].time = tempUITC.i;
//			printf("%d ", tempUITC.i);
			tempITC.c[0] = pint[0].c[0];
			tempITC.c[1] = pint[0].c[1];
			tempITC.c[2] = pint[0].c[2];
			tempITC.c[3] = pint[0].c[3];
			SeeTargetPosList[buffer[0]].update = tempITC.i;
//			printf("%d\r\n", tempITC.i);
//			printf("SeeTargetPos: %f %f %f %d\r\n", SeeTargetPosList[buffer[0]].Angle, SeeTargetPosList[buffer[0]].x, SeeTargetPosList[buffer[0]].y, SeeTargetPosList[buffer[0]].update);
//			if (SeeTargetPosList[buffer[0]].update != 0 && changeFlag)
//			{
//				changeFlag = 0;
//        tempITC.i = 0;
//				pint[0].c[0] = tempITC.c[0]; 
//				pint[0].c[1] = tempITC.c[1]; 
//				pint[0].c[2] = tempITC.c[2]; 
//				pint[0].c[3] = tempITC.c[3];
//				
//				arm_copy_q7(buffer, &SendTargetATPos[LIS_START], 21);
//				xQueueSend(CC2500TxQueue, SendTargetATPos, portMAX_DELAY);
//				SendTime[buffer[0]] = Timer2Counter;
//			}
		}
		
		if (Timer2Counter - LastCheck > 1000)
		{
			LastCheck = Timer2Counter;
			for (i = 0; i < TARGETNUM; i++)
			{
				if ((Timer2Counter - SeeTargetPosList[i].time > 5000 && SeeTargetPosList[i].update == 1) || (Timer2Counter - SeeTargetPosList[i].time > 10000 && SeeTargetPosList[i].update == 0))
				{
					SeeTargetPosList[i].x = -1.0;
					SeeTargetPosList[i].y = -1.0;
					SeeTargetPosList[i].update = 0;
					
					//TargetPositionList[i].x
					TargetPositionList[i].Angle = 0;
					TargetPositionList[i].x = -1.0;
					TargetPositionList[i].y = -1.0;
				}
				
				
			}
			
			for (i = 0; i < TARGETNUM; i++)
			{
				if (1 == SeeTargetPosList[i].update && Timer2Counter - SendTime[i] > 1000)
				{
					buffer[0] = i;
					
					tempFTC.f = SeeTargetPosList[i].Angle;
					pfloatT[0].c[0] = tempFTC.c[0];
					pfloatT[0].c[1] = tempFTC.c[1];
					pfloatT[0].c[2] = tempFTC.c[2];
					pfloatT[0].c[3] = tempFTC.c[3];
					
					tempFTC.f = SeeTargetPosList[i].x;
					pfloatT[1].c[0] = tempFTC.c[0];
					pfloatT[1].c[1] = tempFTC.c[1];
					pfloatT[1].c[2] = tempFTC.c[2];
					pfloatT[1].c[3] = tempFTC.c[3];
					
					tempFTC.f = SeeTargetPosList[i].y;
					pfloatT[2].c[0] = tempFTC.c[0];
					pfloatT[2].c[1] = tempFTC.c[1];
					pfloatT[2].c[2] = tempFTC.c[2];
					pfloatT[2].c[3] = tempFTC.c[3];
					
					tempUITC.i = SeeTargetPosList[i].time;
					puint[0].c[0] = tempUITC.c[0];
					puint[0].c[1] = tempUITC.c[1];
					puint[0].c[2] = tempUITC.c[2];
					puint[0].c[3] = tempUITC.c[3];
					
					tempITC.i = 0;
					pint[0].c[0] = tempITC.c[0]; 
					pint[0].c[1] = tempITC.c[1]; 
					pint[0].c[2] = tempITC.c[2]; 
					pint[0].c[3] = tempITC.c[3];
					
					arm_copy_q7(buffer, &SendTargetATPos[LIS_START], 21);
					xQueueSend(CC2500TxQueue, SendTargetATPos, portMAX_DELAY);
					SendTime[buffer[0]] = Timer2Counter;
				}
			}
		}
	}
}