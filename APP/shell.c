#include "shell.h"
#include <string.h>
#include "Q_Shell.h"
#include "arm_math.h"
#include "target_assign.h"
#include "network.h"
#include "Timer2.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "croutine.h"
#include "semphr.h"

char rxBuffer[INBUFFERSIZE] = {0};
char InBuffer[INBUFFERNUM][INBUFFERSIZE];
IO_Pool InPool = {0, 0, INBUFFERNUM, INBUFFERSIZE, &InBuffer[0][0]};

char OutBuffer[OUTBUFFERNUM][OUTBUFFERSIZE];
IO_Pool OutPool = {0, 0, OUTBUFFERNUM, OUTBUFFERSIZE, &OutBuffer[0][0]};


extern xSemaphoreHandle TargetInformationCounterMutex, TargetInformationEmpty, TargetInformationFull;
extern xSemaphoreHandle SelfTargetInformationMutex;
extern char SelfTargetInformation[];

void Com2Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
//	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  
	 /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
    /* Enable UART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	/* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	/* Configure USART Tx and Rx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode =  USART_Mode_Tx | USART_Mode_Rx;
	
	/* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; //System runs in preemption priority 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
//	DMA_DeInit(DMA1_Stream5);
//	
//	/* Configure DMA Initialization Structure */
//  DMA_InitStructure.DMA_BufferSize = INBUFFERSIZE ;
//  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
//  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
//  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART2->DR)) ;
//  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	
//	 /* Configure RX DMA */
//  DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
//  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)rxBuffer ; 
	
//  /* Enable the DMA Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	/* DMA configuration */
//  DMA_Init(DMA1_Stream5,&DMA_InitStructure);
	
//	/* DMA Interrupt config */
//	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE); 
	
  /* USART configuration */
  USART_Init(USART2, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
//	/* DMA Interrupt init */
//	NVIC_Init(&NVIC_InitStructure);
    
  /* Enable USART */
  USART_Cmd(USART2, ENABLE);
	
//	//Task control 
//	/* Enable DMA USART RX Stream */
//  DMA_Cmd(DMA1_Stream5,ENABLE);
//  
//  /* Enable USART DMA RX Requsts */
//  USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	
//	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
}

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
	
	USART2->SR;
  USART_SendData(USART2, (uint8_t) ch);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}
  return ch;
}

char *GetBuffer(IO_Pool *pool)
{
	int temp;
	if (NULL == pool->ppool)
	{
		return NULL;
	}
	else
	{
		if (pool->front == (pool->rear +1) % pool->length)
		{
			return NULL;
		}
		else
		{
			temp = pool->rear;
			pool->rear = (pool->rear + 1) % pool->length;
			return pool->ppool + (temp * pool->width);
		}
	}
}

char *FreeBuffer(IO_Pool *pool)
{
	int temp;
	if (pool->front == pool->rear)
	{
		return NULL;
	}
	else
	{
		temp = pool->front;
		pool->front = (pool->front + 1) % pool->length;
		return pool->ppool + (temp * pool->width);
	}
}

int shelltest(const signed char * const p)
{
	printf("%s", p);
	return 0;
}
QSH_FUN_REG (shelltest, "int shelltest(const signed char * const p)");



int TargetEfficiency(char *p)
{
	int i;
	char *pBuffer = NULL, tempBuffer[TARGETINFORMATIONLENGTH], temp;
	tempBuffer[0] = SELFADDRESS;
	pBuffer = p;
	i = 0;
	while ('\0' != *pBuffer)
	{
		while (*pBuffer < '0' || *pBuffer > '9')
		{
			pBuffer++;
		}
		sscanf(pBuffer, "%d", &temp);
		i++;
		tempBuffer[i] = temp;
		while (*pBuffer >= '0' && *pBuffer <= '9')
		{
			pBuffer++;
		}
	}
	
	if (TARGETNUM == i)
	{
		if (pdTRUE == xSemaphoreTake(TargetInformationEmpty, portMAX_DELAY))
		{
			if (pdTRUE == xSemaphoreTake(TargetInformationCounterMutex, portMAX_DELAY))
			{				
				if (pBuffer = GetBuffer(&TargetInformationPool))
				{
					arm_copy_q7(tempBuffer, pBuffer, TARGETINFORMATIONLENGTH);
					xSemaphoreGive(TargetInformationFull);
				}
				xSemaphoreGive(TargetInformationCounterMutex);
			}
		}
		
		if (pdTRUE == xSemaphoreTake(SelfTargetInformationMutex, portMAX_DELAY))
		{
			arm_copy_q7(tempBuffer, SelfTargetInformation, TARGETINFORMATIONLENGTH);
			xSemaphoreGive(SelfTargetInformationMutex);
		}
		
	}
	else
	{
		printf("The target number is not match.\r\n");
	}
	return 0;
}
QSH_FUN_REG (TargetEfficiency, "int TargetEfficiency(char *p)");
/*
extern xQueueHandle PIDACKQueue;
extern xSemaphoreHandle GetPacketPoolCounterMutex, GetPacketPoolEmpty, GetPacketPoolFull;
extern xSemaphoreHandle SendPacketPoolCounterMutex, SendPacketPoolEmpty, SendPacketPoolFull;

int SendPIDcmd(float ChangeAngle, int SetSpeed)
{
	char *pBuffer = NULL;
	u32 *pu32 = NULL;
	float *pfloat = NULL;
	int *pint = NULL;
	
	PIDACKQueue = xQueueCreate(1, 4);
	
	if (pdTRUE == xSemaphoreTake(SendPacketPoolEmpty, portMAX_DELAY))
	{
		if (pdTRUE == xSemaphoreTake(SendPacketPoolCounterMutex, portMAX_DELAY))
		{
			pBuffer = GetBuffer(&SendPacketPool);
			xSemaphoreGive(SendPacketPoolCounterMutex);
			
			pBuffer[DES_NUM] = 0x00;
			pBuffer[SRC_NUM] = SELFADDRESS;
			pBuffer[PRC_NUM] = GYROPIDCMDACK;
			pfloat = (float*) &pBuffer[LIS_START];
			pint = (int*) &pBuffer[LIS_START + 4];
			pu32 = (u32*) &pBuffer[1];
			*pfloat = ChangeAngle;
			*pint = SetSpeed;
			pu32[CRC_LENGTH - 3] = Timer2Counter;
			xSemaphoreGive(SendPacketPoolFull);
		}
	}

 
}
 */