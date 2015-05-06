#include "main.h"

void vCycleRecoTask(void *pvParameters)
{
	int Result;
	while (1)
	{
		Result = AssignResult[OrderIndex[SELFADDRESS]];
		//printf("Result: %d",Result);
		if (-1 != Result && !ComfirAlreadyflag)
		{
			//printf("Result: %d",Result);
			//printf("I get\r\n");
			if (pdTRUE == xSemaphoreTake(MoveStopEnable, portMAX_DELAY))
			{
				USART2->SR;
				USART_SendData(UART4, '!');
				xSemaphoreGive(MoveStopEnable);
				
				xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
			}
		}
		vTaskDelay(50);
	}
}
