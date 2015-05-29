#include "network.h"
#include "arm_math.h"
#include "cc2500.h"
#include "Timer2.h"
#include "RNG.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "target_assign.h"
#include "control.h"
#include "Public.h"

extern xSemaphoreHandle TargetInformationCounterMutex, TargetInformationEmpty, TargetInformationFull;
extern xSemaphoreHandle GetPacketPoolCounterMutex, GetPacketPoolEmpty, GetPacketPoolFull;
extern xSemaphoreHandle SendPacketPoolCounterMutex, SendPacketPoolEmpty, SendPacketPoolFull;

void (*Process[])() = {
	                     PreDiscovery,
                       PreDiscoveryAC,
                       NULL,
											 DiscoveryAC,
											 DiscoveryAC,
											 DiscoveryAC};

u8 NodeFlag[LIMITNODENUM + 2]={0, 1, 1, 1, 1, 1, 1, 1, 1};
u8 NbrList[LIMITNODENUM + 2]={8, 1, 2, 3, 4, 5, 6, 7, 8};
u8 gPacketbuf[PACKET_LEN]={'\0'};
u8 sPacketbuf[PACKET_LEN]={0};
vu8 NbrCount = 0, DiscoveryRequest = 0, RSSI, LQI, DiscoveryACFlag = 0, DelayFlag = 0, OSStart = 0, DiscoveryHelp = 0;
vu32 ReceiveTime = 0;
int NodeTimestamp[256] = {0};

char GetPacketBuffer[GETPACKETBUFFERNUM][PACKET_LEN];
IO_Pool GetPacketPool = {0, 0, GETPACKETBUFFERNUM, PACKET_LEN, &GetPacketBuffer[0][0]};

char SendPacketBuffer[SENDPACKETBUFFERNUM][PACKET_LEN];
IO_Pool SendPacketPool = {0, 0, SENDPACKETBUFFERNUM, PACKET_LEN, &SendPacketBuffer[0][0]};

extern xSemaphoreHandle GetPacketPoolCounterMutex, GetPacketPoolEmpty, GetPacketPoolFull;

extern xQueueHandle CC2500RxQueue;
void NETWORK_IRQ_HOOK()
{
	char *Buffer = NULL;
	static signed portBASE_TYPE xHigherPriorityTaskWoken = pdTRUE;
	if (CC2500_PacketCheck())
	{//printf("get\r\n");
		if (OSStart)
		{		/*	
			if (pdTRUE == xSemaphoreTakeFromISR(GetPacketPoolEmpty, NULL))
			{				
				if (pdTRUE == xSemaphoreTakeFromISR(GetPacketPoolCounterMutex, NULL))
				{
					Buffer = GetBuffer(&GetPacketPool);
					xSemaphoreGiveFromISR(GetPacketPoolCounterMutex, NULL);
					
					arm_copy_q7(gPacketbuf, Buffer, PACKET_LEN);
					xSemaphoreGiveFromISR(GetPacketPoolFull, NULL);
				}
				else
				{
					xSemaphoreGiveFromISR(GetPacketPoolEmpty, NULL);
				}
			}	
       */
      xQueueSendFromISR(CC2500RxQueue, gPacketbuf, &xHigherPriorityTaskWoken);	 
		}
		else
		{
			Process[gPacketbuf[PRC_NUM]](NULL);
		}
	}
	DelayFlag=0;
}


bool CC2500_PacketCheck()
{
	u8 i;
	u32 *pu32 = NULL;
	
	RSSI = CC2500_ReadStatus(CCxxx0_RSSI);
	LQI = CC2500_ReadStatus(CCxxx0_LQI);
	
	CC2500_ReceivePacket(gPacketbuf);
	
	ReceiveTime = Timer2Counter;
	
	CC2500_SetRxd();		

  pu32 = (u32*) &gPacketbuf[1];
	CRC_ResetDR();
	if (CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1) == pu32[CRC_LENGTH - 1])
	{		
		if (NodeTimestamp[gPacketbuf[SRC_NUM]] != pu32[CRC_LENGTH - 2])
		{
			NodeTimestamp[gPacketbuf[SRC_NUM]] = pu32[CRC_LENGTH - 2];		
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

/* Process the discovery message before the OS start. */
void PreDiscovery(void *pvParameters)
{
	int i;
	if (NbrCount < MAXNUM)
	{
		if (0 == NodeFlag[gPacketbuf[SRC_NUM]])
		{
			NodeFlag[gPacketbuf[SRC_NUM]] = 1;
			NbrList[++NbrCount] = gPacketbuf[SRC_NUM];
		}
		
		for (i = 0; i < gPacketbuf[LEN_NUM]; i++)
		{
			if (0 == NodeFlag[gPacketbuf[LIS_START + i]])
			{
				NodeFlag[gPacketbuf[LIS_START + i]] = 1;
				NbrList[++NbrCount] = gPacketbuf[LIS_START + i];
			}
		}
		if (gPacketbuf[LEN_NUM] < NbrCount)
		{
			DiscoveryHelp = 1;
		}
	}
	else
	{
		DiscoveryRequest = 1;
	}
}


/* Process the discovery accomplished message before the OS start. */
void PreDiscoveryAC(void *pvParameters)
{
	int i;
	if (NbrCount < MAXNUM)
	{
		if (0 == NodeFlag[gPacketbuf[SRC_NUM]])
		{
			NodeFlag[gPacketbuf[SRC_NUM]] = 1;
			NbrList[++NbrCount] = gPacketbuf[SRC_NUM];
		}
		
		for (i = 0; i < gPacketbuf[LEN_NUM]; i++)
		{
			if (0 == NodeFlag[gPacketbuf[LIS_START + i]])
			{
				NodeFlag[gPacketbuf[LIS_START + i]] = 1;
				NbrList[++NbrCount] = gPacketbuf[LIS_START + i];
			}
		}
	}
	else
	{
		DiscoveryACFlag = 1;
	}
}

/*
void Discovery(void *pvParameters)
{
	int i, sendcounter = 0;
	u32 *pu32 = NULL;
	
	pu32 = (u32*) &sPacketbuf[1];
	pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
	sendcounter = REPEATTIME;
	while (0 != sendcounter)
	{
		if (ISIDLE)
		{
			vTaskDelay(1);
			if (ISIDLE)
			{
				if (Bernoulli(1.0/(MAXNUM - 1)))
				{
					NbrList[0] = NbrCount;
					sPacketbuf[PRC_NUM] = DISCOVERY_ACCOMPLISHED;
					arm_copy_q7(NbrList, sPacketbuf+LEN_NUM, NbrList[0]+1);
					CRC_ResetDR();
					pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);
					CC2500_SendPacket(sPacketbuf, PACKET_LEN);
					CC2500_SetRxd();
				}
				sendcounter--;
			}
		}
		
		vTaskDelay(10);
	}
}
*/

void Discovery(void *pvParameters)
{
	char *pBuffer = NULL;
	if (pdTRUE == xSemaphoreTake(SendPacketPoolEmpty, portMAX_DELAY))
	{
		if (pdTRUE == xSemaphoreTake(SendPacketPoolCounterMutex, portMAX_DELAY))
		{
			pBuffer = GetBuffer(&SendPacketPool);
			xSemaphoreGive(SendPacketPoolCounterMutex);
			
			pBuffer[DES_NUM] = 0x00;
			pBuffer[SRC_NUM] = SELFADDRESS;
			pBuffer[PRC_NUM] = DISCOVERY_ACCOMPLISHED;
			pBuffer[LEN_NUM] = NbrCount;
			arm_copy_q7(NbrList + 1, &pBuffer[LIS_START], NbrList[0]);
			//arm_copy_q7(&inBuffer[0], &pBuffer[LIS_START], TARGETINFORMATIONLENGTH);
			xSemaphoreGive(SendPacketPoolFull);
		}
	}		
}
void DiscoveryAC(void *pvParameters)
{
	
}

void TargetInformaion(void *pvParameters)
{
	u8 *pinBuffer;
	u8 *pBuffer = NULL;
	pinBuffer = (u8*) pvParameters;
	
	if (pdTRUE == xSemaphoreTake(TargetInformationEmpty, portMAX_DELAY))
	{
		if (pdTRUE == xSemaphoreTake(TargetInformationCounterMutex, portMAX_DELAY))
		{
			if (pBuffer = GetBuffer(&TargetInformationPool))
			{
				xSemaphoreGive(TargetInformationCounterMutex);
				
	      arm_copy_q7(&pinBuffer[LIS_START], &pBuffer[0], pinBuffer[LEN_NUM]);
				xSemaphoreGive(TargetInformationFull);
			}		
		}
	}
	
}

extern xQueueHandle TraceNodeQueue;
void TraceExchange(void *pvParameters)
{
	u8 *pinBuffer;
	float *pfloat;
	TraceNode tempNode;
	
	pinBuffer = (u8*) pvParameters;
	pfloat = (float*) &pinBuffer[LIS_START];
	if (0 != pinBuffer[1])
	{
		tempNode.next = NULL;
		tempNode.ID = pinBuffer[SRC_NUM];
		tempNode.x1 = pfloat[0];
		tempNode.y1 = pfloat[1];
		tempNode.x2 = pfloat[2];
		tempNode.y2 = pfloat[3];
		tempNode.UpdateTime = Timer2Counter;
		
//		printf("ID %X %f %f %f %f\r\n", tempNode.ID, tempNode.x1, tempNode.y1, tempNode.x2, tempNode.y2);
		
		xQueueSend(TraceNodeQueue, &tempNode, portMAX_DELAY);
	}
	else
	{
		printf("Wrong message!\r\n");
	}
}

extern xQueueHandle TargetATPosQueue;
void SeeTargetPosExchange(void *pvParameters)
{
//	char buffer[20 + 1];
	char *pinBuffer;
//	TargetATPos *pTATP;
//	
//	FloatToChar *pfloatT, tempFTC;
//	IntToChar *pint, tempITC;
	UIntToChar *puint, tempUITC;
	
	pinBuffer = (char*) pvParameters;
	if (pinBuffer[LEN_NUM] > 0)
	{
		pinBuffer = &pinBuffer[LIS_START];
		
		puint = (UIntToChar*) &pinBuffer[1];
		puint = (UIntToChar*) &puint[3];
		tempUITC.i = Timer2Counter;
		
		puint->c[0] = tempUITC.c[0];
	  puint->c[1] = tempUITC.c[1];
		puint->c[2] = tempUITC.c[2];
		puint->c[3] = tempUITC.c[3];
		
//		printf("other see\r\n");
//	pfloatT = (FloatToChar*) &pinBuffer[1];
//	puint = (UIntToChar*) &pfloatT[3];
//	pint = (IntToChar*) &pfloatT[4];
//	
//	tempITC.c[0] = pint
//	tempITC.c[0]
//	tempITC.c[0]
//	tempITC.c[0]
//	
//	arm_copy_q7(pinBuffer, buffer, sizeof (TargetATPos) + 1);
//	pTATP = (TargetATPos*) &buffer[1];
//	pTATP->update = 0;
//	printf("TargetATPos: %d %f %f %f\r\n", buffer[0], pTATP->Angle, pTATP->x, pTATP->y);
	  xQueueSend(TargetATPosQueue, pinBuffer, portMAX_DELAY);
	}
	
}

void ConfluentEfficiencyExchange(void *pvParameters)
{
	char *pinBuffer = (char*) pvParameters;
	char *poutBuffer = NULL;
	pinBuffer = &pinBuffer[LIS_START];
	
	if (pdTRUE == xSemaphoreTake(TargetInformationEmpty, portMAX_DELAY))
	{
		if (pdTRUE == xSemaphoreTake(TargetInformationCounterMutex, portMAX_DELAY))
		{				
			if (poutBuffer = GetBuffer(&TargetInformationPool))
			{
				arm_copy_q7(pinBuffer, poutBuffer, TARGETNUM + 1);
				xSemaphoreGive(TargetInformationCounterMutex);
			}
			xSemaphoreGive(TargetInformationFull);
		}
	}
//  printf("TargetInformation: %d %d %d %d %d %d %d\r\n", pinBuffer[0],pinBuffer[1],pinBuffer[2],pinBuffer[3],pinBuffer[4],pinBuffer[5],pinBuffer[6]);
}

void SelfSend()
{
	sPacketbuf[DES_NUM] = SELFADDRESS;
	sPacketbuf[SRC_NUM] = SELFADDRESS;
	
	CC2500_SeleChannel(12);
	CC2500_SetRecvAddr(SELFADDRESS);

	CC2500_SetRxd();
	
	CC2500_SendPacket(sPacketbuf,PACKET_LEN);
	CC2500_SetRxd();
	
	sPacketbuf[DES_NUM]=0x00;
}
/*
float SetAngle;

void GyroPIDCmd(void *pvParameters)
{
	float temp, *pfloat;
	int *pint;
	u32 *pu32r = NULL;
	u32 *pu32t = NULL;
	u8 *pu8 = NULL;
	char *pBuffer = NULL;
	
	pu8 = (u8*) pvParameters;
	pfloat = (float*) &pu8[LIS_START];
	pint = (int*) &pu8[LIS_START + 4];
	pu32r = (u32*) &pu8[1];
	
	temp = SetAngle + *pfloat;
	if (temp > 180)
	{
		temp = (temp - 180) - 180;
	}
	if (temp < -180)
	{
		temp = (temp + 180) + 180;
	}
	SetAngle = temp;
	BaseSpeed = *pint;
  
	if (pdTRUE == xSemaphoreTake(SendPacketPoolEmpty, portMAX_DELAY))
	{
		if (pdTRUE == xSemaphoreTake(SendPacketPoolCounterMutex, portMAX_DELAY))
		{
			pBuffer = GetBuffer(&SendPacketPool);
			xSemaphoreGive(SendPacketPoolCounterMutex);
			
			pBuffer[DES_NUM] = pu8[SRC_NUM];
			pBuffer[SRC_NUM] = SELFADDRESS;
			pBuffer[PRC_NUM] = GYROPIDCMDACK;
			pu32t = (u32*) &pBuffer[1];
			pu32t[CRC_LENGTH - 3] = pu32r[CRC_LENGTH - 3];
			xSemaphoreGive(SendPacketPoolFull);
		}
	}		
}

void GyroPIDCmdACK(void *pvParameters)
{
	u32 *pu32r = NULL;
	u8 *pu8 = NULL;
	pu8 = (u8*) pvParameters;
	
}
*/
/* This function runs before the OS start. */
void NetworkConnecting()
{
	int i, p, test, test1;
	int sendcounter = 0;
	u32 *pu32 = NULL;
	
	test1 = Timer2Counter;
	test = Timer2Counter;
	
	
	sPacketbuf[DES_NUM] = SELFADDRESS;
	sPacketbuf[SRC_NUM] = SELFADDRESS;
	
	CC2500_SeleChannel(0);
	CC2500_SetRecvAddr(SELFADDRESS);

	CC2500_SetRxd();
	
	CC2500_SendPacket(sPacketbuf,PACKET_LEN);
	CC2500_SetRxd();
	
	sPacketbuf[DES_NUM]=0x00;
	NbrList[0]=NbrCount;
	
//	while(RESET==RNG_GetFlagStatus(RNG_FLAG_DRDY));
//	Delay_100us(RNG_GetRandomNumber() & 0x000000f);
  pu32 = (u32*) &sPacketbuf[1];
	while (0 == NodeFlag[SELFADDRESS])
	{
		if (ISIDLE)
		{
			NbrList[0] = NbrCount;
			sPacketbuf[PRC_NUM] = DISCOVERY_MESSAGE;
			arm_copy_q7(NbrList, sPacketbuf+LEN_NUM, NbrList[0]+1);
			pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
			CRC_ResetDR();
			pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);
			p = Bernoulli(1.0/(MAXNUM-NbrCount+1));
			//Delay_100us(1);
			if (ISIDLE)
			{
				if (0 == DelayFlag)
				{
					if (p/*Bernoulli(1.0/(MAXNUM-NbrCount+1))*/)
					{
						
						CC2500_SendPacket(sPacketbuf, PACKET_LEN);
						CC2500_SetRxd();
						DelayFlag=1;
					}
				}
				else
				{
					Preemptive_Delay_100us(25, &DelayFlag);
					DelayFlag=0;
				}
			}
		}
	
	}
	
	
	test1 = Timer2Counter - test1;
	while(MAXNUM > NbrCount)
	{
		if (0 != DiscoveryHelp)
		{
			if (ISIDLE)
			{
				Delay_100us(1);
				if (ISIDLE)
				{	
          if (Bernoulli(1.0/(MAXNUM-NbrCount+1)))
					{						
						NbrList[0] = NbrCount;
						sPacketbuf[PRC_NUM] = DISCOVERY_MESSAGE;
						arm_copy_q7(NbrList, sPacketbuf+LEN_NUM, NbrList[0]+1);
						pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
						CRC_ResetDR();
						pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);
						CC2500_SendPacket(sPacketbuf, PACKET_LEN);
						CC2500_SetRxd();
					}
				}
			}	
			DiscoveryHelp = 0;
		}
		
	}
	
	test = Timer2Counter - test;
	while(RESET==RNG_GetFlagStatus(RNG_FLAG_DRDY));
	Delay_100us(RNG_GetRandomNumber() & 0x000000f);
	
	pu32[CRC_LENGTH - 2] = Timer2Counter;//Add the timestamp.
	sendcounter = 3;
	while (0 != sendcounter)
	{
		if (ISIDLE)
		{
			Delay_100us(1);
			if (ISIDLE)
			{				
				NbrList[0] = NbrCount;
				sPacketbuf[PRC_NUM] = DISCOVERY_ACCOMPLISHED;
				arm_copy_q7(NbrList, sPacketbuf+LEN_NUM, NbrList[0]+1);
				CRC_ResetDR();
				pu32[CRC_LENGTH - 1] = CRC_CalcBlockCRC(pu32, CRC_LENGTH - 1);
				CC2500_SendPacket(sPacketbuf, PACKET_LEN);
				CC2500_SetRxd();
				sendcounter--;
			}
		}	
		Delay_100us(10);
	}
	
	printf("NeighbourList:\r\n");
	for (i=0;i<MAXNUM;i++)
	{
		printf("%#x\r\n",NbrList[1+i]);
	}
  printf("t1 = %d\r\n", test1);
	printf("t = %d\r\n", test);
}