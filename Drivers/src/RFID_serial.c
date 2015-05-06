#include "RFID_serial.h"
#include <absacc.h>

char RFID_ID[IDLENTH]__at(0x2001c000 + 0x50);

void Com6Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  
	/* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* Enable USART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  
  /* USART3 GPIO configuration -----------------------------------------------*/ 
  /* Connect USART pins to AF7 */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode =  USART_Mode_Tx | USART_Mode_Rx;
	
	/* USART configuration */
  USART_Init(USART6, &USART_InitStructure);
	
	/* Enable the USARTx Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; //System runs in preemption priority 4;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

//	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

  /* Configure DMA controller to manage USART TX and RX DMA request ----------*/ 
   
  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = IDLENTH;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART6->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
  /* Configure TX DMA */
//  DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
//  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)RFID_ID;
//	
//  DMA_Init(DMA1_Stream3,&DMA_InitStructure);
	
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = DMA_Channel_5 ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)RFID_ID ; 
  DMA_Init(DMA2_Stream1,&DMA_InitStructure);
	
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
	
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
  /* Enable USART */
  USART_Cmd(USART6, ENABLE);
	

}
/*
unsigned int IDtoInt(char* ID)
{
	unsigned int id = 0;
	int i;
	for (i = 0; i < 8; i++)
	{
		if (ID[i] >= '0' && ID[i] <= '9')
		{
			id = id + (ID[i] - '0');
		}
		else
		{
			if (ID[i] >= 'A' && ID[i] <= 'F')
			{
				id = id + (ID[i] - 'A' + 10);
			}
			else
			{
				return 0;
			}
		}	
		if (i < 7)
		{
			id = id << 4;
		}
	}
	return id;
}
*/