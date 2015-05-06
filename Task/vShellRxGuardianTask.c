#include "main.h"

void vShellRxGuardianTask(void *pvParameters)
{
	char *pBuffer = NULL, inBuffer[INBUFFERSIZE];
	
	for (;;)
	{
		if (pdTRUE == xSemaphoreTake(InPoolFull, portMAX_DELAY))
		{
			if (pdTRUE == xSemaphoreTake(InPoolCounterMutex, portMAX_DELAY))
			{				
				if (pBuffer = FreeBuffer(&InPool))
				{
					xSemaphoreGive(InPoolCounterMutex);
					
					arm_copy_q7((q7_t*) pBuffer, (q7_t*) inBuffer, INBUFFERSIZE);
					arm_fill_q7(0, (q7_t*) pBuffer, INBUFFERSIZE);
					xSemaphoreGive(InPoolEmpty);
				}			
			}
		}
		if (pBuffer)
		{
			pBuffer = NULL;
			Q_Sh_CmdHandler(1, inBuffer);
		}
	}
}
