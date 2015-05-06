#include "main.h"

void vCompassReadTask(void *pvParameters)
{
	volatile float temp;
	volatile float offset;
	int i;
	for (i = 0; i < 200; i++)
	{
		offset = COMP_Read();
		COMP_PreRead();
		vTaskDelay(10);
	}

	while(1)
	{
		COMP_PreRead();
		vTaskDelay(10);
		temp = COMP_Read();
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
		//printf("%f\r\n", CurAngle);
		
//		if (ANGLE_CORRECTION_THRESHOLD < (fabs(CurAngle - CurPos.Angle) > 180 ? 360 - fabs(CurAngle - CurPos.Angle) : fabs(CurAngle - CurPos.Angle)))
//		{
//			if (pdTRUE == xSemaphoreTake(ForwardStopEnable, 0))
//			{
//				USART2->SR;
//				USART_SendData(UART4, '!');
//				xSemaphoreGive(ForwardStopEnable);
//			}
//		}
	}
}
