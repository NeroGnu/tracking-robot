/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides all exceptions handler and peripherals interrupt
  *          service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
// void SVC_Handler(void)
// {
// }

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
// void PendSV_Handler(void)
// {
// }

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
// void SysTick_Handler(void)
// {
// }

/******************************************************************************/
/*                 STM32Fxxx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32fxxx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  This function handles EXTI0_IRQ Handler.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
void OTG_FS_WKUP_IRQHandler(void)
{
}

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{
}

/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
vu32 uhRxCounter = 0;
void USART2_IRQHandler(void)
{
	char *Buffer = NULL;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    rxBuffer[uhRxCounter] = (USART_ReceiveData(USART2) & 0x7F);
		if (0x0d == rxBuffer[uhRxCounter])
		{
			rxBuffer[uhRxCounter] = '\0';
			
			uhRxCounter = 0;
			if (pdTRUE == xSemaphoreTakeFromISR(InPoolEmpty, NULL))
			{
				
				if (pdTRUE == xSemaphoreTakeFromISR(InPoolCounterMutex, NULL))
				{
					Buffer = GetBuffer(&InPool);
					arm_copy_q7(rxBuffer, Buffer, INBUFFERSIZE);
					xSemaphoreGiveFromISR(InPoolFull, NULL);
					xSemaphoreGiveFromISR(InPoolCounterMutex, NULL);
				}
			}
			//printf("%s",rxBuffer);
		}
		else
		{
			uhRxCounter++;
		}
    
  }
}
	
	
/**
* @brief  This function handles CC2500 global interrupt request.
* @param  None
* @retval None
*/

void EXTI4_IRQHandler(void)
{
	//printf("EXTI4_IRQHandler\r\n");
	//ReceiveTime = Timer2Counter;
	printf("\n");
  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {		
		
		NETWORK_IRQ_HOOK();
		EXTI_ClearITPendingBit(EXTI_Line4);
  }
	
}

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
		
		Timer2Counter++;
		if (Timer2Counter % 10000 == 0)		//	1s
		{
			second_Counter++;
		}
	}
}

/**
  * @brief  This function handles DMA1_Stream1 global interrupt request.
  * @param  None
  * @retval None
  */
	
void DMA1_Stream1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);  
    DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_TCIF1);
		USART_ClearFlag(USART3, USART_FLAG_TC);
		//printf("in\r\n");
    xSemaphoreGiveFromISR(BCRxSyn, NULL);
		
		DMA_Cmd(DMA1_Stream1,ENABLE);
		USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
  }
}

/**
  * @brief  This function handles DMA2_Stream1 global interrupt request.
  * @param  None
  * @retval None
  */

void DMA2_Stream1_IRQHandler(void)
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken = pdTRUE;
	if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);  
    DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);
		USART_ClearFlag(USART6, USART_FLAG_TC);
    xSemaphoreGiveFromISR(TagSyn, &xHigherPriorityTaskWoken);
		
		DMA_Cmd(DMA2_Stream1,ENABLE);
		USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);
  }
}


void DMA1_Stream2_IRQHandler(void)
{
	static int value;
	static signed portBASE_TYPE xHigherPriorityTaskWoken = pdTRUE;
	if(DMA_GetITStatus(DMA1_Stream2, DMA_IT_TCIF2))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);  
    DMA_ClearFlag(DMA1_Stream2, DMA_FLAG_TCIF2);
		USART_ClearFlag(UART4, USART_FLAG_TC);
		value = *((int*) FEEDBACK_VALUE);
		if ('$' == FEEDBACK_VALUE[4])
		{
		  xQueueSendFromISR(ArduinoFeedbackQueue, &value, NULL);
		}
		if ('@' == FEEDBACK_VALUE[4])
		{
			xQueueSendFromISR(ArduinoQueryQueue, &value, &xHigherPriorityTaskWoken);
		}
	  
		DMA_Cmd(DMA1_Stream2, ENABLE);
		USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
  }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
