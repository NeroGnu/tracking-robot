#include "main.h"

void vMovingControlTask(void *pvParameters)
{
	Position TargetPos;
	Position ExpectPos;
	MoveCmd SendCmd, ReResult;
	float DeltAngle, DeltDistance;
	float XYi[3];
	while (1)
	{
		if (pdTRUE == xQueueReceive(TargetPosQueue, &TargetPos, portMAX_DELAY))
		{
//			printf("get TargetPosQueue\r\n");
			while ((fabs(TargetPos.Angle - CurPos.Angle) > 180 ? 360 - fabs(TargetPos.Angle - CurPos.Angle) : fabs(TargetPos.Angle - CurPos.Angle)) * STEP_ANGLE >= ADJUST_STEP_ANGLE\
				|| sqrt(pow(TargetPos.y - CurPos.y, 2) + pow(TargetPos.x - CurPos.x, 2)) * STEP_METER >= ADJUST_STEP_DIRECT)
			{
//				Debug_printf("CurPos: %f %f %f\r\n", CurPos.Angle, CurPos.x, CurPos.y);
//				Debug_printf("CurPos: %f %f %f\r\n", CurPos.Angle, CurPos.x, CurPos.y);
//				Debug_printf("Compass: %f\r\n", CurAngle);
//				Debug_printf("DAngle: %f\r\n", fabs(TargetPos.Angle - CurPos.Angle) * STEP_ANGLE);
//				Debug_printf("Distance: %f\r\n", sqrt(pow(TargetPos.y - CurPos.y, 2) + pow(TargetPos.x - CurPos.x, 2)) * STEP_METER);
				if (sqrt(pow(TargetPos.y - CurPos.y, 2) + pow(TargetPos.x - CurPos.x, 2)) * STEP_METER >= ADJUST_STEP_DIRECT)
				{
					ExpectPos.Angle = (atan2(TargetPos.y - CurPos.y, TargetPos.x - CurPos.x)/PI)*180;
					
//					printf("ExpectPos.Angle1 : %f\r\n",ExpectPos.Angle);
					ExpectPos.x = CurPos.x;
					ExpectPos.y = CurPos.y;
					DeltAngle = ExpectPos.Angle - CurPos.Angle;
					
//					Debug_printf("DAngle: %f\r\n", DeltAngle);
					if (DeltAngle > 180)
					{
						DeltAngle = DeltAngle - 360;
					}                     
					if(DeltAngle < -180)
					{
						DeltAngle = 360 + DeltAngle;
					}
					if (DeltAngle < 0)
					{
						SendCmd.direct = turnright;
					}
					else
					{
						SendCmd.direct = turnleft;
					}
					SendCmd.step = fabs(DeltAngle) * STEP_ANGLE;
//					vTaskDelay(1000);
					xQueueSend(MoveCmdQueue, &SendCmd, portMAX_DELAY);
					if (pdTRUE == xQueueReceive(MoveRsultQueue, &ReResult, portMAX_DELAY))
					{
//						Debug_printf("get MoveRsultQueue\r\n");
						//CurPos = ExpectPos;
						if (SendCmd.direct == turnright)
						{
							CurPos.Angle = CurPos.Angle - ((float)ReResult.step) / STEP_ANGLE;
						}
						if (SendCmd.direct == turnleft)
						{
							CurPos.Angle = CurPos.Angle + ((float)ReResult.step) / STEP_ANGLE;
						}
						
						if (CurPos.Angle > 180)
						{
							CurPos.Angle = CurPos.Angle - 360;
						}
						if (CurPos.Angle < -180)
						{
							CurPos.Angle = CurPos.Angle + 360;
						}
						//??
						//????????;
						//printf("CurPos.Angle: %f\r\n", CurPos.Angle);
						if (ANGLE_CORRECTION_THRESHOLD < fabs(CurPos.Angle - CurAngle))
						{
							CurPos.Angle = CurAngle;
						}
						//printf("FIXED CurPos.Angle: %f\r\n", CurPos.Angle);
						if(pdTRUE == xQueueReceive(PositionUpdatekQueue, XYi, 2))
						{
							//????????;
							switch (GlobleCmdDir)
							{
								case forward:
									XYi[0] = XYi[0] + ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
							    XYi[1] = XYi[1] + ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
									break;
								case backward:
									XYi[0] = XYi[0] - ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
							    XYi[1] = XYi[1] - ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
									break;
								case turnright:			
									break;
								case turnleft:
									break;
								default:
									break;		
							}
							CurPos.x = XYi[0];
							CurPos.y = XYi[1];
						}
						if(pdTRUE == xSemaphoreTake(FoundTargetSyn, 1))
						{
//							Debug_printf("I am IN 1\r\n");
							TargetPos = CurPos;
						}
					}
				}
				else
				{
					ExpectPos.Angle = TargetPos.Angle;
					
//					printf("ExpectPos.Angle2 : %f\r\n",ExpectPos.Angle);
					ExpectPos.x = CurPos.x;
					ExpectPos.y = CurPos.y;
					DeltAngle = ExpectPos.Angle - CurPos.Angle;
					
					
					if (DeltAngle > 180)
					{
						DeltAngle = DeltAngle - 360;
					}                     
					if(DeltAngle < -180)
					{
						DeltAngle = 360 + DeltAngle;
					}
					if (DeltAngle < 0)
					{
						SendCmd.direct = turnright;
					}
					else
					{
						SendCmd.direct = turnleft;
					}
					SendCmd.step = fabs(DeltAngle) * STEP_ANGLE;
//					vTaskDelay(1000);
					xQueueSend(MoveCmdQueue, &SendCmd, portMAX_DELAY);
					if (pdTRUE == xQueueReceive(MoveRsultQueue, &ReResult, portMAX_DELAY))
					{
						//CurPos = ExpectPos;MoveRsultQueue
//						Debug_printf("get MoveRsultQueue\r\n");
						if (SendCmd.direct == turnright)
						{
							CurPos.Angle = CurPos.Angle - ((float)ReResult.step) / STEP_ANGLE;
						}
						if (SendCmd.direct == turnleft)
						{
							CurPos.Angle = CurPos.Angle + ((float)ReResult.step) / STEP_ANGLE;
						}
						
						if (CurPos.Angle > 180)
						{
							CurPos.Angle = CurPos.Angle - 360;
						}
						if (CurPos.Angle < -180)
						{
							CurPos.Angle = CurPos.Angle + 360;
						}
						//??
						//????????;
						//printf(" else CurPos.Angle: %f\r\n", CurPos.Angle);
						if (ANGLE_CORRECTION_THRESHOLD < fabs(CurPos.Angle - CurAngle))
						{
							CurPos.Angle = CurAngle;
						}
						//printf("else FIXED CurPos.Angle: %f\r\n", CurPos.Angle);
						if(pdTRUE == xQueueReceive(PositionUpdatekQueue, XYi, 2))
						{
							//????????;
							switch (GlobleCmdDir)
							{
								case forward:
									XYi[0] = XYi[0] + ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
							    XYi[1] = XYi[1] + ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
									break;
								case backward:
									XYi[0] = XYi[0] - ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
							    XYi[1] = XYi[1] - ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
									break;
								case turnright:			
									break;
								case turnleft:
									break;
								default:
									break;		
							}
							CurPos.x = XYi[0];
							CurPos.y = XYi[1];
						}
						if(pdTRUE == xSemaphoreTake(FoundTargetSyn, 100))
						{
//							Debug_printf("I am IN 2\r\n");
							TargetPos = CurPos;
						}
					}
				}
				
				if ((fabs(ExpectPos.Angle - CurPos.Angle) > 180 ? 360 - fabs(ExpectPos.Angle - CurPos.Angle) : fabs(ExpectPos.Angle - CurPos.Angle)) * STEP_ANGLE < ADJUST_STEP_ANGLE\
					&& sqrt(pow(TargetPos.y - CurPos.y, 2) + pow(TargetPos.x - CurPos.x, 2)) * STEP_METER >= ADJUST_STEP_DIRECT)
				{
					ExpectPos.x = TargetPos.x;
					ExpectPos.y = TargetPos.y;
					DeltDistance = sqrt(pow(ExpectPos.y - CurPos.y, 2) + pow(ExpectPos.x - CurPos.x, 2));
					SendCmd.direct = forward;
					SendCmd.step = DeltDistance * STEP_METER;
//					vTaskDelay(1000);
//          Debug_printf("return1\r\n");
					xQueueSend(MoveCmdQueue, &SendCmd, portMAX_DELAY);
					
					if (pdTRUE == xQueueReceive(MoveRsultQueue, &ReResult, portMAX_DELAY))
					{
//						Debug_printf("return2\r\n");
						//CurPos = ExpectPos;
//						Debug_printf("get MoveRsultQueue\r\n");
						CurPos.x = CurPos.x + (ReResult.step / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
						CurPos.y = CurPos.y + (ReResult.step / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
						
						//??
						//????????;
						if (ANGLE_CORRECTION_THRESHOLD < fabs(CurPos.Angle - CurAngle))
						{
							CurPos.Angle = CurAngle;
						}
						if(pdTRUE == xQueueReceive(PositionUpdatekQueue, XYi, 2))
						{
							//????????;
							switch (GlobleCmdDir)
							{
								case forward:
									XYi[0] = XYi[0] + ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
							    XYi[1] = XYi[1] + ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
									break;
								case backward:
									XYi[0] = XYi[0] - ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_cos_f32((CurPos.Angle / 180) * PI);
							    XYi[1] = XYi[1] - ((float)(ReResult.step - XYi[2]) / STEP_METER) * arm_sin_f32((CurPos.Angle / 180) * PI);
									break;
								case turnright:			
									break;
								case turnleft:
									break;
								default:
									break;		
							}
							CurPos.x = XYi[0];
							CurPos.y = XYi[1];
						}
						if(pdTRUE == xSemaphoreTake(FoundTargetSyn, 1))
						{
//							Debug_printf("I am IN 3\r\n");
							TargetPos = CurPos;
						}
//						Debug_printf("return3\r\n");
					}
				}
				
			}
//			Debug_printf("TargetPos: %f %f %f\r\n", CurPos.Angle, CurPos.x, CurPos.y);
//			Debug_printf("CurPos: %f %f %f\r\n", CurPos.Angle, CurPos.x, CurPos.y);
//			Debug_printf("Compass: %f\r\n", CurAngle);
			xSemaphoreGive(MoveComplete);
//			printf("Complete1\r\n");
		}
	}
}
