
#include "main.h"

int T_TIMER = 0;
void tTimerCallback( xTimerHandle pxTimer )
{
	int D_value;
	T_TIMER = Timer2Counter;
	
	//printf("Timer2Counter:%d\r\n",Timer2Counter);
	
//	D_value = Timer2Counter - ((int)((Timer2Counter + T_TAKE_DATA_TIME/2)/T_TAKE_DATA_TIME))*T_TAKE_DATA_TIME;
	D_value = T_TIMER % T_TAKE_DATA_TIME;
	
	//printf("Timer2Counter:%d\r\n",Timer2Counter);
	//D_value = 10;
	if((D_value < (T_TAKE_DATA_TIME - D_value) ? D_value : (T_TAKE_DATA_TIME - D_value)) < 50)
	{
//		D_value = Timer2Counter - ((int)((Timer2Counter + TAKE_DATA_TIME/2)/TAKE_DATA_TIME))*TAKE_DATA_TIME;
		if( xTimerChangePeriod( xTimers, TAKE_DATA_TIME-(int)((Timer2Counter%T_TAKE_DATA_TIME) < (T_TAKE_DATA_TIME - Timer2Counter%T_TAKE_DATA_TIME) ? (Timer2Counter%T_TAKE_DATA_TIME) : (Timer2Counter%T_TAKE_DATA_TIME - T_TAKE_DATA_TIME))/10, 0 ) == pdPASS )
		{
		}
		
		
		Debug_printf("T_TIMER1:%d\r\n",T_TIMER);
		xSemaphoreGive(TimerxSyn);
		return;
		
	}
	else
	{
		if( xTimerChangePeriod( xTimers, TAKE_DATA_TIME - (int)(Timer2Counter%T_TAKE_DATA_TIME)/10, 0 ) == pdPASS )
		{
				/* The command was successfully sent. */
				Debug_printf("T_TIMER:%d\r\n",T_TIMER);
				Debug_printf("PASS2 %d\r\n",TAKE_DATA_TIME - (int)(D_value/10));
		}
		return;
	}
    
}
