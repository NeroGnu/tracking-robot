#include "main.h"

void vNetworkRxGuardianTask(void *pvParameters)
{
	char *pBuffer = NULL, inBuffer[PACKET_LEN];
	OSStart = 1;
	Process[3] = TraceExchange;
	Process[4] = SeeTargetPosExchange;
	Process[5] = ConfluentEfficiencyExchange;
	CC2500_InterruptEnable(TRUE);
	for (;;)
	{
		
		xQueueReceive(CC2500RxQueue, inBuffer, portMAX_DELAY);
		Process[inBuffer[PRC_NUM]](inBuffer);
		/*
		if (pdTRUE == xSemaphoreTake(GetPacketPoolFull, portMAX_DELAY))
		{
			if (pdTRUE == xSemaphoreTake(GetPacketPoolCounterMutex, portMAX_DELAY))
			{				
				if (pBuffer = FreeBuffer(&GetPacketPool))
				{
					xSemaphoreGive(GetPacketPoolCounterMutex);
					
					arm_copy_q7(pBuffer, inBuffer, PACKET_LEN);
					arm_fill_q7(0, pBuffer, PACKET_LEN);
					xSemaphoreGive(GetPacketPoolEmpty);
				}		
			}
		}
		if (pBuffer)
		{
			pBuffer = NULL;		
			Process[inBuffer[PRC_NUM]](inBuffer);			
		}
		*/
	}
}
