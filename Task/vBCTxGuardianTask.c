#include "main.h"

void vBCTxGuardianTask(void *pvParameters)
{
	char ResultToTx = 0;
	for (;;)
	{
		if (pdTRUE == xQueueReceive(BCTxQueue, &ResultToTx, 100))
		{
			for (;;)
			{
				USART3->SR;
				USART_SendData(USART3, ResultToTx);
				xQueueReceive(BCTxQueue, &ResultToTx, portMAX_DELAY);
				//printf("%d\r\n", ResultToTx);
			}
		}
		else
		{
			USART3->SR;
			USART_SendData(USART3, ResultToTx);
		}
	}
}
