#include "main.h"

void vBCRxGuardianTask(void *pvParameters)
{
	char TempInformation[TARGETINFORMATIONLENGTH] = {SELFADDRESS, 0};

	DMA_Cmd(DMA1_Stream1,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
	for (;;)
	{
		if (pdTRUE == xSemaphoreTake(BCRxSyn, portMAX_DELAY))
		{
//			SerialTranslate();
//			printf("get!");
//		
			ProcessVideoResult();
//			printf("out ProcessVideoResult\r\n");
//			printf("%d\r\n", AccuracyTest());
		}
	}
}
