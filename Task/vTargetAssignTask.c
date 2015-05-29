#include "main.h"

#define LONGESTDIS  565.685
#define PRIVATE_WEIGHT  1.0
#define PUBLIC_WEIGHT   0.0

void vTargetAssignTask(void *pvParameters)
{
	matrix_u8 matrix_efficiency, matrix_ans;
	int efficiency, i, j, ChangeFlag, SelfChangeFlag, Sum;
	float tempDis;
	char *poutBuffer = NULL, ResultToTx = 0x0;
	u8 *pBuffer = NULL, inBuffer[TARGETINFORMATIONLENGTH], tempmax, tempDisToChar;
  int tempindex;
	Init_Matrix(&matrix_efficiency, MAXNUM, TARGETNUM, 0);
	Init_Matrix(&matrix_ans, MAXNUM, TARGETNUM, 0);
	Indexing(OrderIndex, NodeFlag, LIMITNODENUM + 2);
	ChangeFlag = 0;
	SelfChangeFlag = 0;
	Sum = 0;
	
	for (i = 0; i < MAXNUM; i++)
	{
		AssignResult[i] = -1;
	}
	
	for (;;)
	{
		
		if (pdTRUE == xSemaphoreTake(TargetInformationFull, portMAX_DELAY))
		{
			if (pdTRUE == xSemaphoreTake(TargetInformationCounterMutex, portMAX_DELAY))
			{				
				if (pBuffer = FreeBuffer(&TargetInformationPool))
				{
					xSemaphoreGive(TargetInformationCounterMutex);
					
					arm_copy_q7(pBuffer, inBuffer, TARGETINFORMATIONLENGTH);
					//arm_fill_q7(0, pBuffer, TARGETINFORMATIONLENGTH);
					xSemaphoreGive(TargetInformationEmpty);
				}
			}  
		}
		
		if (pBuffer)
		{			
			pBuffer = NULL;
			
			//检测来源，自己的得做处理。
			if (SELFADDRESS == inBuffer[0])
			{
				for (i = 0; i < MAXNUM; i++)
				{
					if (SeeTargetPosList[i].x > 0 || SeeTargetPosList[i].y > 0)
					{
						tempDis = sqrt((SeeTargetPosList[i].x - CurPos.x) * (SeeTargetPosList[i].x - CurPos.x) + (SeeTargetPosList[i].y - CurPos.y) * (SeeTargetPosList[i].y - CurPos.y));
						tempDisToChar = (int)(((LONGESTDIS - tempDis) / LONGESTDIS) * 127);
					}
					else
					{
						tempDisToChar = 0;
					}
					SelfConfluentEfficiency[i + 1] = (int) (PRIVATE_WEIGHT * inBuffer[i + 1] + PUBLIC_WEIGHT * tempDisToChar);
				}
				arm_copy_q7(&SelfConfluentEfficiency[1], &matrix_efficiency.Data[OrderIndex[SelfConfluentEfficiency[0]]][0], TARGETNUM);
				
			}
			else
			{
				for (i = 0; i < MAXNUM; i++)
				{
					if (SeeTargetPosList[i].x > 0 || SeeTargetPosList[i].y > 0)
					{
						tempDis = sqrt((SeeTargetPosList[i].x - CurPos.x) * (SeeTargetPosList[i].x - CurPos.x) + (SeeTargetPosList[i].y - CurPos.y) * (SeeTargetPosList[i].y - CurPos.y));
						tempDisToChar = (int)(((LONGESTDIS - tempDis) / LONGESTDIS) * 127);
					}
					else
					{
						tempDisToChar = 0;
					}
					SelfConfluentEfficiency[i + 1] = (int) (PRIVATE_WEIGHT * inBuffer[i + 1] + PUBLIC_WEIGHT * tempDisToChar);
				}
				arm_copy_q7(&SelfConfluentEfficiency[1], &matrix_efficiency.Data[OrderIndex[SelfConfluentEfficiency[0]]][0], TARGETNUM);
				
//				printf("Other Efficiency\r\n");
				arm_copy_q7(&inBuffer[1], &matrix_efficiency.Data[OrderIndex[inBuffer[0]]][0], TARGETNUM);
			}
//      printf("SelfConfluentEfficiency: %d %d %d %d %d %d\r\n", SelfConfluentEfficiency[1]);//, SelfConfluentEfficiency[2], SelfConfluentEfficiency[3], SelfConfluentEfficiency[4], SelfConfluentEfficiency[5], SelfConfluentEfficiency[6]);
      
			efficiency = TargetAssign(matrix_efficiency, &matrix_ans);
			/*********************************check*****************************/
			Comparison(&matrix_efficiency, &matrix_ans, &second_Counter);
			
			
			/*******************************************************************/
			for (i = 0; i < MAXNUM; i++)
			{
				arm_max_q7(&matrix_ans.Data[i][0], TARGETNUM, &tempmax, &tempindex);
				if (0 == tempindex)
				{
					Sum = 0;
					for (j = 0; j < TARGETNUM; j++)
					{
						Sum = Sum + matrix_ans.Data[i][j];
					}
					//printf("Sum = %d\r\n", Sum);
					if (0 == Sum)
					{
						tempindex = -1;
					}
				}
				
				if ( AssignResult[i] != tempindex)
				{
					ChangeFlag = 1;
					if (i == OrderIndex[SELFADDRESS])
					{
						SelfChangeFlag = 1;
					}
					AssignResult[i] = tempindex;
				}
			}
			//printf("give ComunicationWithBoardSyn\r\n");
			xSemaphoreGive(ComunicationWithBoardSyn);
			
			if (0 != SelfChangeFlag)
			{
				ResultToTx = AssignResult[OrderIndex[SELFADDRESS]] + 1;
				xQueueSend(BCTxQueue, &ResultToTx, portMAX_DELAY);
				SelfChangeFlag = 0;
			}
			
			
			/*
			if (0 != ChangeFlag && SELFADDRESS == inBuffer[0])
			{			
				if (pdTRUE == xSemaphoreTake(SendPacketPoolEmpty, portMAX_DELAY))
				{
					if (pdTRUE == xSemaphoreTake(SendPacketPoolCounterMutex, portMAX_DELAY))
					{
						poutBuffer = GetBuffer(&SendPacketPool);
						xSemaphoreGive(SendPacketPoolCounterMutex);
						
						poutBuffer[DES_NUM] = 0x00;
						poutBuffer[SRC_NUM] = SELFADDRESS;
						poutBuffer[PRC_NUM] = TARGETINFORMATION;
						poutBuffer[LEN_NUM] = TARGETINFORMATIONLENGTH;
						arm_copy_q7(&inBuffer[0], &poutBuffer[LIS_START], TARGETINFORMATIONLENGTH);
						xSemaphoreGive(SendPacketPoolFull);						
					}
					else
					{
						xSemaphoreGive(SendPacketPoolEmpty);
					}
				}
        ChangeFlag = 0;				
			}	
			*/
			//printf("Result = %d\r\n", AssignResult[OrderIndex[SELFADDRESS]]);
/*
     // if (0 != ChangeFlag)
			{
				printf("Efficiency matrix is:\r\n");
				for (i = 0; i < matrix_efficiency.numRows; i++)
				{
					for (j = 0; j < matrix_efficiency.numCols; j++)
					{
						printf("%d, ", matrix_efficiency.Data[i][j]);
					}
					printf("\r\n");
				}
				
				printf("Assault matrix is:\r\n");
				for (i = 0; i < matrix_ans.numRows; i++)
				{
					for (j = 0; j < matrix_ans.numCols; j++)
					{
						printf("%d, ", matrix_ans.Data[i][j]);
					}
					printf("\r\n");
				}
				printf("Efficiency is %d\r\n", efficiency);
				
				//ChangeFlag = 0;
			}
			*/
		}		
	}
}
