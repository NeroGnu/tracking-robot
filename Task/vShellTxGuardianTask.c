#include "main.h"
//#include "Task.h"

void vShellTxGuardianTask(void *pvParameters)
{
	char *pBuffer = NULL, outBuffer[OUTBUFFERSIZE];
	for (;;)
	{
		if (pdTRUE == xSemaphoreTake(OutPoolFull, portMAX_DELAY))
		{
			if (pdTRUE == xSemaphoreTake(OutPoolCounterMutex, portMAX_DELAY))
			{
				if (pBuffer = FreeBuffer(&OutPool))
				{
					xSemaphoreGive(OutPoolCounterMutex);
					
					arm_copy_q7(pBuffer, outBuffer, OUTBUFFERSIZE);
					arm_fill_q7(0, pBuffer, OUTBUFFERSIZE);
					xSemaphoreGive(OutPoolEmpty);
				}			
			}
		}
		if (pBuffer)
		{
			pBuffer = NULL;
			printf("%s", outBuffer);
		}
	}
}