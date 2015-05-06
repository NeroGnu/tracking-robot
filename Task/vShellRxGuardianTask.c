#include "main.h"
//#include "Task.h"

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
					
					arm_copy_q7(pBuffer, inBuffer, INBUFFERSIZE);
					arm_fill_q7(0, pBuffer, INBUFFERSIZE);
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
