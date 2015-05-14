#include "main.h"

void vtime_Test_task(void *pvParameters)
{
	int timeVal;
	int WITHT = 120,HEIGHT = 150;
	Position TargetPos;
	while(1)
	{
		if (pdTRUE == xQueueReceive(TimeTestQueue, &timeVal, portMAX_DELAY))
		{
			while(second_Counter < timeVal * 60)
			{
				srand(Timer2Counter%10000);
				TargetPos.Angle = rand()%360 - 180;
				TargetPos.x = rand() % (WITHT+1);
				TargetPos.y = rand() % (HEIGHT+1);
				Debug_printf("TargetPos: %f %f %f\r\n",TargetPos.Angle,TargetPos.x,TargetPos.y);	
				xQueueSend(TargetPosQueue, &TargetPos, portMAX_DELAY);
				xSemaphoreTake(MoveComplete, portMAX_DELAY);
			}
			TargetPos.Angle = 90;
			TargetPos.x = 0;
			TargetPos.y = 0;
			Debug_printf("TargetPos: %f %f %f\r\n",TargetPos.Angle,TargetPos.x,TargetPos.y);

			xQueueSend(TargetPosQueue, &TargetPos, portMAX_DELAY);
			xSemaphoreTake(MoveComplete, portMAX_DELAY);
			Debug_printf("times of changing angle: %d\r\n",count_change_angle);
			Debug_printf("times of changing pos: %d\r\n",count_change_pos);
		}
	}
}
