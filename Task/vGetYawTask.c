#include "main.h"

void vGetYawTask(void *pvParameters)
{
	u8 StartCmd[]={0xa5, 0x5a, 0x04, 0x15, 0x19, 0xaa};
	u8* pS = NULL;
	int i;
	volatile float temp;
	volatile float offset;
	
	DMA_Cmd(DMA1_Stream0,ENABLE);
	USART_DMACmd(UART5, USART_DMAReq_Rx, ENABLE);
	for (pS = StartCmd; 0xaa != *pS; pS++)
	{
		UART5->SR;
		USART_SendData(UART5, *pS);
		while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET){}
	}
	UART5->SR;
	USART_SendData(UART5, *pS);
	
	for(i = 0; i < 100 ; i++)
	{
		if (pdTRUE == xSemaphoreTake(AHRS_Syn, portMAX_DELAY))
		{
			if (AHRS_Check())
			{
				offset = AHRS_GetYaw();
				printf("Yaw=%f\n", AHRS_GetYaw());
			}
		}
	}
	
	for(;;)
	{
		if (pdTRUE == xSemaphoreTake(AHRS_Syn, portMAX_DELAY))
		{
			if (AHRS_Check())
			{
				temp = AHRS_GetYaw();
				temp = temp - offset + 90;
				
				if (temp > 180)
				{
					temp = temp - 360;
				}
				
				if (temp < -180)
				{
					temp = 360 + temp;
				}
				
				CurAngle = temp > 0 ? (180.0 - temp) : (-180 - temp);
			}
		}
	}
}

