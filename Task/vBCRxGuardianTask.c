#include "main.h"

void vBCRxGuardianTask(void *pvParameters)
{
	char TempInformation[TARGETINFORMATIONLENGTH] = {SELFADDRESS, 0};

	DMA_Cmd(DMA1_Stream1,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
	
	/*********************************** Create timer ***********************************/
	xTimers = xTimerCreate
		          (  /* Just a text name, not used by the RTOS kernel. */
                     (const signed char*)"Timer",
                     /* The timer period in ticks. */
                     ( TAKE_DATA_TIME-(int)((Timer2Counter%T_TAKE_DATA_TIME) < (T_TAKE_DATA_TIME - Timer2Counter%T_TAKE_DATA_TIME) ? (Timer2Counter%T_TAKE_DATA_TIME) : (Timer2Counter%T_TAKE_DATA_TIME - T_TAKE_DATA_TIME))/10 ),
                     /* The timers will auto-reload themselves when they expire. */
                     pdTRUE,
                     /* Assign each timer a unique id equal to its array index. */
                     NULL,
                     /* Each timer calls the same callback when it expires. */
                     tTimerCallback
                   );

         if( xTimers == NULL )
         {
             /* The timer was not created. */
						 Debug_printf("timer created fail\r\n");
         }
         else
         {
             /* Start the timer.  No block time is specified, and even if one was
             it would be ignored because the RTOS scheduler has not yet been
             started. */
             if( xTimerStart( xTimers, 0 ) != pdPASS )
             {
							 Debug_printf("timer started fail\r\n");
                 /* The timer could not be set into the Active state. */
             }
         }
				 
				 
	for (;;)
	{
		if (pdTRUE == xSemaphoreTake(BCRxSyn, portMAX_DELAY))
		{
//			SerialHex();
//			SerialTranslate();
//			printf("get!");
//		
			ProcessVideoResult();
//			printf("out ProcessVideoResult\r\n");
//			printf("%d\r\n", AccuracyTest());
		}
	}
}
