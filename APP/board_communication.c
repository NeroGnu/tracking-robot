#include "board_communication.h"
#include <absacc.h>
#include "main.h"

//#define TEST
extern xSemaphoreHandle TargetInformationCounterMutex, TargetInformationEmpty, TargetInformationFull;
extern xSemaphoreHandle SelfTargetInformationMutex;
extern char SelfTargetInformation[];

extern xSemaphoreHandle SelfXCoordinateSyn;
extern FloatToChar SelfXCoordinate[TARGETNUM];
extern FloatToChar SelfXDistance[TARGETNUM];

//char BCTxBuffer[TARGETNUM][TARGETINFORLEN]__at(0x2001c000);
#ifdef TEST
char BCRxBuffer[RE_TARGETNUM][TARGETINFORLEN]__at(0x2001c000 + 0x18);
#else
char BCRxBuffer[BOARDRXLEN]__at(0x2001c000); //
//char BCRxBuffer[BOARDRXLEN];
#endif

void BoardCommunication_Config(void)
{
#ifdef TEST
  int i, j;
#endif
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* Enable USART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
  /* USART3 GPIO configuration -----------------------------------------------*/ 
  /* Connect USART pins to AF7 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
  /* USART3 configuration ----------------------------------------------------*/
  /* Enable the USART OverSampling by 8 */
  USART_OverSampling8Cmd(USART3, ENABLE); 
  
  /* USART3 configured as follow:
        - BaudRate = 5250000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 8
		     is: (USART APB Clock / 8) 
			 Example: 
			    - (USART3 APB1 Clock / 8) = (42 MHz / 8) = 5250000 baud
			    - (USART1 APB2 Clock / 8) = (84 MHz / 8) = 10500000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 16
		     is: (USART APB Clock / 16) 
			 Example: (USART3 APB1 Clock / 16) = (42 MHz / 16) = 2625000 baud
			 Example: (USART1 APB2 Clock / 16) = (84 MHz / 16) = 5250000 baud
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */ 
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* When using Parity the word length must be configured to 9 bits */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure);

  /* Configure DMA controller to manage USART TX and RX DMA request ----------*/ 
   
  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = BOARDRXLEN;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART3->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
  /* Configure TX DMA */
//  DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
//  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)BCTxBuffer;
//	
//  DMA_Init(DMA1_Stream3,&DMA_InitStructure);
	
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)BCRxBuffer ; 
  DMA_Init(DMA1_Stream1,&DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);
	
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USART3, ENABLE);
	
#ifdef TEST
  for (i = 0; i < RE_TARGETNUM; i++)
	{
		for (j = 0; j < TARGETINFORLEN; j++)
		{
			BCTxBuffer[i][j] = j;
		}
	}
#endif
}

int AccuracyTest(void)
{
#ifdef TEST
	int i, j, flag;
	flag = 1;
	for (i = 0; i < RE_TARGETNUM; i++)
	{
		for (j = 0; j < TARGETINFORLEN; j++)
		{
			if (BCRxBuffer[i][j] != BCTxBuffer[i][j])
			{
				flag = 0;
				break;
			}
		}
		if (0 == flag)
		{
			break;
		}
	}
	return flag;
#endif
}

extern volatile float CurAngle;
void ProcessVideoResult()
{
	char *pBuffer = NULL;
	FloatToChar *pfloat = NULL;
	int i, receivetime;
	int ChangeFlag = 0;
	float deltax, deltay;
	//TargetATPos *pTATP;
	char buffer[20 + 1];
	static char LastEfficiency[TARGETNUM + 1] = {SELFADDRESS, 0};
	static unsigned int FrameCounter = 0;
	static int UpdateCounter[TARGETNUM] = {0};
	
	FloatToChar *pfloatT, tempFTC;
	IntToChar *pint, tempITC;
	UIntToChar *puint, tempUITC;
	
	pfloatT = (FloatToChar*) &buffer[1];
	puint = (UIntToChar*) &pfloatT[3];
	pint = (IntToChar*) &pfloatT[4];
	
	//pTATP = (TargetATPos*) &buffer[1];
	
	pfloat = (FloatToChar*) &BCRxBuffer[RE_TARGETNUM];
	
	receivetime = Timer2Counter;
	
	
	//printf("0\r\n");
	if (pdTRUE == xSemaphoreTake(SelfTargetInformationMutex, portMAX_DELAY))
	{
		arm_copy_q7(BCRxBuffer, SelfTargetInformation + 1, TARGETNUM);
		xSemaphoreGive(SelfTargetInformationMutex);
	}
	FrameCounter++;
	
	for (i = 0; i < TARGETNUM; i++)
	{
		if (LastEfficiency[1 + i] != BCRxBuffer[1 + i])
		{
			ChangeFlag = 1;
			break;
		}
	}
	
	arm_copy_q7(BCRxBuffer, LastEfficiency + 1, TARGETNUM);
	
	//printf("1\r\n");
	//printf("SelfTargetInformation: ");
	for (i = 0; i < TARGETNUM; i++)
	{
//		printf("SelfTargetInformation[%d]:%d \r\n",i,SelfTargetInformation[i + 1]);
		SelfXCoordinate[i] = pfloat[i];
		SelfXDistance[i].c[0] = pfloat[RE_TARGETNUM + i].c[0];
		SelfXDistance[i].c[1] = pfloat[RE_TARGETNUM + i].c[1];
		SelfXDistance[i].c[2] = pfloat[RE_TARGETNUM + i].c[2];
		SelfXDistance[i].c[3] = pfloat[RE_TARGETNUM + i].c[3];
		if (0 < (signed char)BCRxBuffer[i] || SelfXDistance[i].f > 0.001)
		{
			//printf("2\r\n");
//			printf("SelfXDistance :%f \r\n",SelfXDistance[i].f);
			Globaldis = SelfXDistance[i].f;
			TargetPositionList[i].Angle = CurPos.Angle + (float) computeAngle(SelfXDistance[i].f , 240 - SelfXCoordinate[i].f);
			deltax = SelfXDistance[i].f * arm_cos_f32((TargetPositionList[i].Angle / 180) * PI);
			deltay = SelfXDistance[i].f * arm_sin_f32((TargetPositionList[i].Angle / 180) * PI);
			TargetPositionList[i].x = CurPos.x + deltax;
			TargetPositionList[i].y = CurPos.y + deltay;
			//printf("2\r\n");
			buffer[0] = i;
			//printf("2\r\n");
//			printf("TargetPositionList[i].Angle: %f %f %f\r\n", TargetPositionList[i].Angle, TargetPositionList[i].x, TargetPositionList[i].y);
//			printf("SeePos: ");
			tempFTC.f = TargetPositionList[i].Angle;
			pfloatT[0].c[0] = tempFTC.c[0];
			pfloatT[0].c[1] = tempFTC.c[1];
			pfloatT[0].c[2] = tempFTC.c[2];
			pfloatT[0].c[3] = tempFTC.c[3];
//			printf("%f ", tempFTC.f);
			tempFTC.f = CurPos.x + deltax - 30 * arm_cos_f32((TargetPositionList[i].Angle / 180) * PI);
			pfloatT[1].c[0] = tempFTC.c[0];
			pfloatT[1].c[1] = tempFTC.c[1];
			pfloatT[1].c[2] = tempFTC.c[2];
			pfloatT[1].c[3] = tempFTC.c[3];
//			printf("%f ", tempFTC.f);
			tempFTC.f = CurPos.y + deltay - 30 * arm_sin_f32((TargetPositionList[i].Angle / 180) * PI);
			pfloatT[2].c[0] = tempFTC.c[0];
			pfloatT[2].c[1] = tempFTC.c[1];
			pfloatT[2].c[2] = tempFTC.c[2];
			pfloatT[2].c[3] = tempFTC.c[3];
//			printf("%f ", tempFTC.f);
			tempUITC.i = receivetime;
			puint[0].c[0] = tempUITC.c[0];
			puint[0].c[1] = tempUITC.c[1];
			puint[0].c[2] = tempUITC.c[2];
			puint[0].c[3] = tempUITC.c[3];
//			printf("%d ", tempUITC.i);
			tempITC.i = 1;
			pint[0].c[0] = tempITC.c[0];
			pint[0].c[1] = tempITC.c[1];
			pint[0].c[2] = tempITC.c[2];
			pint[0].c[3] = tempITC.c[3];
//			printf("%d\r\n", tempITC.i);
//			printf("before TargetATPosQueue\r\n");
			xQueueSend(TargetATPosQueue, buffer, portMAX_DELAY);
//			printf("after TargetATPosQueue\r\n");
		}
		else
		{
			UpdateCounter[i]++;
			if (10 < UpdateCounter[i])
			{
				UpdateCounter[i] = 0;
   			TargetPositionList[i].Angle = 0;
  			TargetPositionList[i].x = -1.0;
  			TargetPositionList[i].y = -1.0;
			}
//			TargetPositionList[i].Angle = 0;
//			TargetPositionList[i].x = -1.0;
//			TargetPositionList[i].y = -1.0;
		}
	}
	//printf("\r\n");
	
	if (1 == ChangeFlag || FrameCounter > 10)
	{
		ChangeFlag = 0;
		FrameCounter = 0;
//		printf("wake decision\r\n");
		if (pdTRUE == xSemaphoreTake(TargetInformationEmpty, portMAX_DELAY))
		{
			if (pdTRUE == xSemaphoreTake(TargetInformationCounterMutex, portMAX_DELAY))
			{				
				if (pBuffer = GetBuffer(&TargetInformationPool))
				{
					pBuffer[0] = SELFADDRESS;
					arm_copy_q7(BCRxBuffer, pBuffer + 1, TARGETNUM);
					xSemaphoreGive(TargetInformationCounterMutex);
					
				}
				xSemaphoreGive(TargetInformationFull);
			}
		}
	}
	
		xSemaphoreGive(SelfXCoordinateSyn);
} 

void SerialTranslate()
{
	int i;
	FloatToChar temp;
	float *pfloat = NULL;
	pfloat = (float*) &BCRxBuffer[6];
	printf("Data\r\n");
	for (i = 0; i < 6; i++)
	{
		printf("%d, ", BCRxBuffer[i]);
	}
	printf("\r\n");
	
	for (i = 0; i < 6; i++)
	{
		temp.c[0] = BCRxBuffer[i*4+6+0];
		temp.c[1] = BCRxBuffer[i*4+6+1];
		temp.c[2] = BCRxBuffer[i*4+6+2];
		temp.c[3] = BCRxBuffer[i*4+6+3];
		
		printf("%f, ", temp.f);
	}
	printf("\r\n");
	
	for (i = 0; i < 6; i++)
	{
		temp.c[0] = BCRxBuffer[i*4+30+0];
		temp.c[1] = BCRxBuffer[i*4+30+1];
		temp.c[2] = BCRxBuffer[i*4+30+2];
		temp.c[3] = BCRxBuffer[i*4+30+3];
		
		printf("%f, ", temp.f);
	}
	printf("\r\n");
}

void ConvertionByWeight(char *src, char *result)
{
	int i;
	float temp1[TARGETNUM] = {0};
  float temp2[TARGETNUM] = {0};
	float *psrc = (float*) src;
	
	arm_offset_f32(psrc, -240, temp1, TARGETNUM);
	arm_abs_f32(temp1, temp1, TARGETNUM);
	arm_scale_f32(temp1, (1000000.0/240.0)*0.7, temp1, TARGETNUM);
	
	arm_mult_f32(psrc + 2*TARGETNUM, psrc + 3*TARGETNUM, temp2, TARGETNUM);
	arm_scale_f32(temp2, (1000000.0/(480*272))*0.3, temp2, TARGETNUM);
	
	arm_add_f32(temp1, temp2, temp1, TARGETNUM);
	arm_scale_f32(temp1, 127.0/1000000.0, temp1, TARGETNUM);
	
	for (i = 0; i < TARGETNUM; i++)
	{
		result[i] = (char) temp1[i];
	}
}