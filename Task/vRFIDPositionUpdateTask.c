#include "main.h"

void vRFIDPositionUpdateTask(void *pvParameters)
{
	int id;
	float XYi[3], temp;
	int QueryResult;
	Position TempPos;
	DMA_Cmd(DMA2_Stream1,ENABLE);
	USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);
	
	while(1)
	{
		if (pdTRUE == xSemaphoreTake(TagSyn, portMAX_DELAY))
		{
			USART2->SR;
			USART_SendData(UART4, '?');
			IntIDtoXY(IDArray, 124, IDtoInt(RFID_ID), &XYi[0], &XYi[1]);
			xQueueReceive(ArduinoQueryQueue, &QueryResult, portMAX_DELAY);
			TempPos = CurPos;
			switch (GlobleCmdDir)
			{
				case forward:
					TempPos.x = CurPos.x + (QueryResult / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
					TempPos.y = CurPos.y + (QueryResult / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
					break;
				case backward:
					TempPos.x = CurPos.x - (QueryResult / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
					TempPos.y = CurPos.y - (QueryResult / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
					break;
				case turnright:			
					break;
				case turnleft:
					break;
				default:
          break;		
			}
			temp = (TempPos.x - XYi[0]) * (TempPos.x - XYi[0]) + (TempPos.y - XYi[1]) * (TempPos.y - XYi[1]);
			arm_sqrt_f32(temp, &temp);
			
//			printf("(X,Y)=(%f,%f)\r\n",XYi[0],XYi[1]);
			if (POSITION_CORRECTION_THRESHOLD < temp)
			{
				if (pdTRUE == xSemaphoreTake(MoveStopEnable, portMAX_DELAY))
				{
					USART2->SR;
					USART_SendData(UART4, '!');
					xSemaphoreGive(MoveStopEnable);
					
					XYi[2] = QueryResult;
					xQueueSend(PositionUpdatekQueue, XYi, portMAX_DELAY);
				}
			}
		}
	}
}
