#include "main.h"

void vStepmotorDriveTask(void *pvParameters)
{
	char CmdStr[20]={0};
  MoveCmd ReceivedCmd, Result;
	
	DMA_Cmd(DMA1_Stream2,ENABLE);
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
  
	
	while (1)
	{
		if (pdTRUE == xQueueReceive(MoveCmdQueue, &ReceivedCmd, portMAX_DELAY))
		{
//			printf("get MoveCmdQueue\r\n");
			GlobleCmdDir = ReceivedCmd.direct;
			if (ReceivedCmd.step > 0)
			{
				switch(ReceivedCmd.direct)
				{
					case forward:
//						printf("in forward 0 %u\r\n", ReceivedCmd.step);
						sprintf(CmdStr, "forward %u", ReceivedCmd.step);
//						printf("in forward 1\r\n");
						CmdSend(CmdStr, strlen(CmdStr), '$');
//						printf("%s\r\n", CmdStr);
						xSemaphoreGive(MoveStopEnable);
						xSemaphoreGive(ForwardStopEnable);
						arm_fill_q7(0, CmdStr, 20);
						xQueueReceive(ArduinoFeedbackQueue, &Result.step, portMAX_DELAY);
//						printf("get ArduinoFeedbackQueue\r\n");
						xSemaphoreTake(ForwardStopEnable, portMAX_DELAY);
						xSemaphoreTake(MoveStopEnable, portMAX_DELAY);
						
						Result.direct = ReceivedCmd.direct;
						//printf("%d\r\n", Result.step);
						xQueueSend(MoveRsultQueue, &Result, portMAX_DELAY);
						break;
					case backward:
//						printf("in backward 0\r\n");
						sprintf(CmdStr, "backward %u", ReceivedCmd.step);
						CmdSend(CmdStr, strlen(CmdStr), '$');
						
						xSemaphoreGive(MoveStopEnable);
						arm_fill_q7(0, CmdStr, 20);
						xQueueReceive(ArduinoFeedbackQueue, &Result.step, portMAX_DELAY);
						xSemaphoreTake(MoveStopEnable, portMAX_DELAY);
						
						Result.direct = ReceivedCmd.direct;
						xQueueSend(MoveRsultQueue, &Result, portMAX_DELAY);
						break;
					case turnright:
//						printf("in turnright 0 %u\r\n", ReceivedCmd.step);
						sprintf(CmdStr, "turnright %u", ReceivedCmd.step);
//						printf("in turnright 1\r\n");
						CmdSend(CmdStr, strlen(CmdStr), '$');
//						printf("%s\r\n", CmdStr);
						xSemaphoreGive(MoveStopEnable);
						arm_fill_q7(0, CmdStr, 20);
						xQueueReceive(ArduinoFeedbackQueue, &Result.step, portMAX_DELAY);
						xSemaphoreTake(MoveStopEnable, portMAX_DELAY);
						
						Result.direct = ReceivedCmd.direct;
						xQueueSend(MoveRsultQueue, &Result, portMAX_DELAY);
						break;
					case turnleft:
//						printf("in turnleft 0 %u\r\n", ReceivedCmd.step);
						sprintf(CmdStr, "turnleft %u", ReceivedCmd.step);
//						printf("in turnleft 1\r\n");
						CmdSend(CmdStr, strlen(CmdStr), '$');
						
						xSemaphoreGive(MoveStopEnable);
						arm_fill_q7(0, CmdStr, 20);
						xQueueReceive(ArduinoFeedbackQueue, &Result.step, portMAX_DELAY);
						xSemaphoreTake(MoveStopEnable, portMAX_DELAY);
						
						Result.direct = ReceivedCmd.direct;
						xQueueSend(MoveRsultQueue, &Result, portMAX_DELAY);
						break;
					default:
//						printf("in default 0\r\n");
						break;					
				}
			}
			else
			{
				xQueueSend(MoveRsultQueue, &ReceivedCmd, portMAX_DELAY);
			}
//			printf("arduino return\r\n");
		}
	}
}
