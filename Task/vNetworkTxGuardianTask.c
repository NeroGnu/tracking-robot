#include "main.h"

#define CYCLE_TIME (0x00004ff)

void vNetworkTxGuardianTask(void *pvParameters)
{
	int RoutineDelay = 0;
	int sendcounter, timecounter = 0;
	int TimeoutCounter = 0;
	unsigned int t1, t2;
	unsigned int T_TraceRoutineBuffer, T_SendTargetATPosBuffer = 0;
	u32 *pu32;
	float *pfloat;
	unsigned char outBuffer[PACKET_LEN], TraceRoutineBuffer[PACKET_LEN], SendTargetATPosBuffer[PACKET_LEN], SelfConfluentEfficiencyBuffer[PACKET_LEN];
	TraceRoutineBuffer[DES_NUM] = 0x00;
	TraceRoutineBuffer[SRC_NUM] = SELFADDRESS;
	TraceRoutineBuffer[PRC_NUM] = 0x03;
	TraceRoutineBuffer[LEN_NUM] = 0x00;
	
	SendTargetATPosBuffer[DES_NUM] = 0x00;
	SendTargetATPosBuffer[SRC_NUM] = SELFADDRESS;
	SendTargetATPosBuffer[PRC_NUM] = 0x04;
	SendTargetATPosBuffer[LEN_NUM] = 0x00;
	
	SelfConfluentEfficiencyBuffer[DES_NUM] = 0x00;
	SelfConfluentEfficiencyBuffer[SRC_NUM] = SELFADDRESS;
	SelfConfluentEfficiencyBuffer[PRC_NUM] = 0x05;
	SelfConfluentEfficiencyBuffer[LEN_NUM] = TARGETNUM + 1;
	
	while(RESET==RNG_GetFlagStatus(RNG_FLAG_DRDY));
	RoutineDelay = RNG_GetRandomNumber() & CYCLE_TIME;
	
	for (;;)
	{
		t1 = Timer2Counter;
		if (pdTRUE == xQueueReceive(CC2500TxQueue, outBuffer, RoutineDelay))
		{
//			printf("send!\r\n");
			pu32 = (u32*)&outBuffer[1];
			pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
			CRC_ResetDR();
			pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);//Add the CRC check byte.
			sendcounter = REPEATTIME;
			while (0 != sendcounter)
			{
				CC2500_SendPacket(outBuffer, PACKET_LEN);
				CC2500_SetRxd();
				sendcounter--;
				vTaskDelay(10);
			}
			
			switch (outBuffer[PRC_NUM])
			{
				case 0x03:
					arm_copy_q7(outBuffer, TraceRoutineBuffer, PACKET_LEN);
					T_TraceRoutineBuffer = Timer2Counter;
					break;
				case 0x04:
					arm_copy_q7(outBuffer, SendTargetATPosBuffer, PACKET_LEN);
					T_SendTargetATPosBuffer = Timer2Counter;
					break;
				default:
					break;
			}
      t2 = Timer2Counter;
			timecounter = timecounter + t2 - t1;
			
			if (CYCLE_TIME <= timecounter)
			{
				timecounter = 0;
//				printf("send Routine!!\r\n");
				//Trace
				if (TraceRoutineBuffer[LEN_NUM] > 0)
				{
					pu32 = (u32*)&TraceRoutineBuffer[1];
					pfloat = (float*)&TraceRoutineBuffer[LIS_START];
					pfloat[0] = CurPos.x;
					pfloat[1] = CurPos.y;
					pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
					CRC_ResetDR();
					pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);//Add the CRC check byte.
					sendcounter = REPEATTIME;
					while (0 != sendcounter)
					{
						CC2500_SendPacket(TraceRoutineBuffer, PACKET_LEN);
						CC2500_SetRxd();
						sendcounter--;
						vTaskDelay(10);
					}
				}
				
				//SendTargetATPosBuffer
				if (SendTargetATPosBuffer[LEN_NUM] > 0)
				{
					pu32 = (u32*)&SendTargetATPosBuffer[1];
					pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
					CRC_ResetDR();
					pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);//Add the CRC check byte.
					sendcounter = REPEATTIME;
					while (0 != sendcounter)
					{
						CC2500_SendPacket(SendTargetATPosBuffer, PACKET_LEN);
						CC2500_SetRxd();
						sendcounter--;
						vTaskDelay(10);
					}
				}
				
				//SelfConfluentEfficiency
				pu32 = (u32*)&SelfConfluentEfficiencyBuffer[1];
				arm_copy_q7(SelfConfluentEfficiency, &SelfConfluentEfficiencyBuffer[LIS_START], TARGETNUM + 1);
				pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
				CRC_ResetDR();
				pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);//Add the CRC check byte.
				sendcounter = REPEATTIME;
				while (0 != sendcounter)
				{
					CC2500_SendPacket(SelfConfluentEfficiencyBuffer, PACKET_LEN);
					CC2500_SetRxd();
					sendcounter--;
					vTaskDelay(4);
				}
				
			}
		}
		else
		{
			timecounter = timecounter + RoutineDelay;
			if (CYCLE_TIME <= timecounter)
			{
				timecounter = 0;
//				printf("send Routine!!\r\n");
				//Trace
				if (TraceRoutineBuffer[LEN_NUM] > 0)
				{
					pu32 = (u32*)&TraceRoutineBuffer[1];
					pfloat = (float*)&TraceRoutineBuffer[LIS_START];
					pfloat[0] = CurPos.x;
					pfloat[1] = CurPos.y;
					pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
					CRC_ResetDR();
					pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);//Add the CRC check byte.
					sendcounter = REPEATTIME;
					while (0 != sendcounter)
					{
						CC2500_SendPacket(TraceRoutineBuffer, PACKET_LEN);
						CC2500_SetRxd();
						sendcounter--;
						vTaskDelay(10);
					}
				}
				
				//SendTargetATPosBuffer
				if (SendTargetATPosBuffer[LEN_NUM] > 0)
				{
					pu32 = (u32*)&SendTargetATPosBuffer[1];
					pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
					CRC_ResetDR();
					pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);//Add the CRC check byte.
					sendcounter = REPEATTIME;
					while (0 != sendcounter)
					{
						CC2500_SendPacket(SendTargetATPosBuffer, PACKET_LEN);
						CC2500_SetRxd();
						sendcounter--;
						vTaskDelay(10);
					}
				}
				
				//SelfConfluentEfficiency
				pu32 = (u32*)&SelfConfluentEfficiencyBuffer[1];
				arm_copy_q7(SelfConfluentEfficiency, &SelfConfluentEfficiencyBuffer[LIS_START], TARGETNUM + 1);
				pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
				CRC_ResetDR();
				pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);//Add the CRC check byte.
				sendcounter = REPEATTIME;
				while (0 != sendcounter)
				{
					CC2500_SendPacket(SelfConfluentEfficiencyBuffer, PACKET_LEN);
					CC2500_SetRxd();
					sendcounter--;
					vTaskDelay(10);
				}
				
			}
			RoutineDelay = CYCLE_TIME - RoutineDelay;
		}
		
		if (Timer2Counter - T_TraceRoutineBuffer > 5000)
		{
			TraceRoutineBuffer[LEN_NUM] = 0x00;
		}
		
		if (Timer2Counter - T_SendTargetATPosBuffer > 5000)
		{
			SendTargetATPosBuffer[LEN_NUM] = 0x00;
		}
	}
}