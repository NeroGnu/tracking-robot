//******************************************************************************
#include "main.h"

//******************************************************************************


//******************************************************************************

/* Uart IO pool resource semaphore. */
xSemaphoreHandle OutBufferMutex, OutPoolNum, InPoolEmpty, InPoolCounterMutex, InPoolFull;
xSemaphoreHandle OutPoolEmpty, OutPoolCounterMutex, OutPoolFull;

/* Network send and receive pool resource semaphore. */
xSemaphoreHandle GetPacketPoolCounterMutex, GetPacketPoolEmpty, GetPacketPoolFull;
xSemaphoreHandle SendPacketPoolCounterMutex, SendPacketPoolEmpty, SendPacketPoolFull;

/* Target informaion queue semaphore. */
xSemaphoreHandle TargetInformationCounterMutex, TargetInformationEmpty, TargetInformationFull;
xSemaphoreHandle SelfTargetInformationMutex;

/* Board communication recieve and send synchronous semaphore. */
xSemaphoreHandle BCRxSyn, BCTxSyn;

/* Self X coordinate semaphore.*/
xSemaphoreHandle SelfXCoordinateSyn;

/* Tag synchronization.*/
xSemaphoreHandle TagSyn;

xSemaphoreHandle ComunicationWithBoardSyn;

xSemaphoreHandle ComunicationWithTraceSyn;

xSemaphoreHandle CorrectAngleSyn;

/* Move complete synchronization.*/
xSemaphoreHandle MoveComplete;

/* Forward stop enable */
xSemaphoreHandle ForwardStopEnable;

/* Move stop enable */
xSemaphoreHandle MoveStopEnable;


/*if the timer's callback function is called ,than give the Semaphore*/
xSemaphoreHandle TimerxSyn;

/* AHRS data synchronization */
xSemaphoreHandle AHRS_Syn;

/*find the target*/
xSemaphoreHandle FoundTargetSyn;

/* System output semaphore */
xSemaphoreHandle OutputMutex;

//****************************************************


/* Move command queue.*/
xQueueHandle MoveCmdQueue;

/* Move result queue.*/
xQueueHandle MoveRsultQueue;

/* Target Position queue.*/
xQueueHandle TargetPosQueue;

/* Arduino feedback queue.*/
xQueueHandle ArduinoFeedbackQueue;

/* Arduino query queue.*/
xQueueHandle ArduinoQueryQueue;

/* Position update queue.*/
xQueueHandle PositionUpdatekQueue;

/* Board communication Tx queue. */
xQueueHandle BCTxQueue;

/* Trace node queue. */
xQueueHandle TraceNodeQueue;

/* CC2500 Rx Queue. */
xQueueHandle CC2500RxQueue;

/* CC2500 Tx Queue. */
xQueueHandle CC2500TxQueue;

/* TargetATPos Queue. */
xQueueHandle TargetATPosQueue;

xQueueHandle TimeTestQueue;

/*timer*/
xTimerHandle xTimers;

//******************************************************************************
char SelfTargetInformation[TARGETINFORMATIONLENGTH] = {SELFADDRESS, 0};
FloatToChar SelfXCoordinate[TARGETNUM];
FloatToChar SelfXDistance[TARGETNUM];

volatile float CurAngle = 0;

volatile int goflag = 0, ComfirAlreadyflag = 0;
int count_change_angle = 0, count_change_pos = 0 ,isFindOver = 1;

///////////////////////////////////////////////////////////
Position TargetPositionList[TARGETNUM];
TargetATPos SeeTargetPosList[TARGETNUM];
char SelfConfluentEfficiency[TARGETNUM + 1] = {SELFADDRESS, 0};

//extern vu32 second_Counter;
//extern Position GoThroght[60];
//******************************************************************************


int main(void)
{
	/*!< At this stage the microcontroller clock setting is already configured,
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f4xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f4xx.c file
	 */
	
	/*!< Most systems default to the wanted configuration, with the noticeable 
		exception of the STM32 driver library. If you are using an STM32 with 
		the STM32 driver library then ensure all the priority bits are assigned 
		to be preempt priority bits by calling 
		NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 ); before the RTOS is started.
	*/
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* Enable CRC clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	
	Timer2Init();
	Com2Init();
	printf("Init OK!\r\n");
	Com4Init();
	AHRS_Com5Init();
	Com6Init();
	
	RNG_Init();
//	Delay_100us(650);
//	COMP_Init();
//	COMP_PreRead();

	CC2500_Init();
	SelfSend();
	CC2500_InterruptEnable(FALSE);
	
	
	Delay_100us(10000);
//	NetworkConnecting();
//	printf("Network OK!\r\n");

  BoardCommunication_Config();
	
	/*********************************** Create semaphore ***********************************/
	BCRxSyn = xSemaphoreCreateCounting(1, 0);
	BCTxSyn = xSemaphoreCreateCounting(1, 0);
	
	OutPoolEmpty = xSemaphoreCreateCounting(OUTBUFFERNUM - 1, OUTBUFFERNUM - 1);
	OutPoolFull = xSemaphoreCreateCounting(OUTBUFFERNUM - 1, 0);
	OutPoolCounterMutex = xSemaphoreCreateMutex();
	
	InPoolEmpty = xSemaphoreCreateCounting(INBUFFERNUM - 1, INBUFFERNUM - 1);
	InPoolFull = xSemaphoreCreateCounting(INBUFFERNUM - 1, 0);
	InPoolCounterMutex = xSemaphoreCreateMutex();
	
  GetPacketPoolEmpty = xSemaphoreCreateCounting(GETPACKETBUFFERNUM - 1, GETPACKETBUFFERNUM - 1);
	GetPacketPoolFull = xSemaphoreCreateCounting(GETPACKETBUFFERNUM - 1, 0);
	GetPacketPoolCounterMutex = xSemaphoreCreateMutex();
	
	SendPacketPoolEmpty = xSemaphoreCreateCounting(SENDPACKETBUFFERNUM - 1, SENDPACKETBUFFERNUM - 1);
	SendPacketPoolFull = xSemaphoreCreateCounting(SENDPACKETBUFFERNUM - 1, 0);
	SendPacketPoolCounterMutex = xSemaphoreCreateMutex();

  TargetInformationEmpty = xSemaphoreCreateCounting(TARGETINFORMATIONQUEUE - 1, TARGETINFORMATIONQUEUE - 1);
	TargetInformationFull = xSemaphoreCreateCounting(TARGETINFORMATIONQUEUE - 1, 0);
	TargetInformationCounterMutex = xSemaphoreCreateMutex();
	
	SelfTargetInformationMutex = xSemaphoreCreateMutex();
	SelfXCoordinateSyn = xSemaphoreCreateCounting(1, 0);
	
	TagSyn = xSemaphoreCreateCounting(1, 0);
	
	CorrectAngleSyn = xSemaphoreCreateCounting(1, 0);
	MoveComplete = xSemaphoreCreateCounting(1, 0);
	
	ForwardStopEnable = xSemaphoreCreateCounting(1, 0);
	MoveStopEnable = xSemaphoreCreateCounting(1, 0);
	
	FoundTargetSyn = xSemaphoreCreateCounting(1, 0);
	
	ComunicationWithBoardSyn = xSemaphoreCreateCounting(1, 0);
	
	ComunicationWithTraceSyn = xSemaphoreCreateCounting(1, 0);
	AHRS_Syn = xSemaphoreCreateCounting(1, 0);
	
	OutputMutex = xSemaphoreCreateMutex();
	TimerxSyn = xSemaphoreCreateCounting(1, 0);
	/*********************************** Create queue ***********************************/
	BCTxQueue = xQueueCreate(1, 1);
	
	MoveCmdQueue = xQueueCreate(1, sizeof (MoveCmd));
	
	MoveRsultQueue = xQueueCreate(1, sizeof (MoveCmd));
	
	TargetPosQueue = xQueueCreate(1, sizeof (Position));
	
	ArduinoFeedbackQueue = xQueueCreate(1, sizeof (int));
	
	ArduinoQueryQueue = xQueueCreate(1, sizeof (int));
	
	PositionUpdatekQueue = xQueueCreate(1, sizeof (float)*3);
	
	TraceNodeQueue = xQueueCreate(2, sizeof (TraceNode));
	
	CC2500RxQueue = xQueueCreate(5, PACKET_LEN);
	
	CC2500TxQueue = xQueueCreate(5, PACKET_LEN);
	
	TargetATPosQueue = xQueueCreate(10, 20 + 1);
	
	TimeTestQueue = xQueueCreate(1, sizeof (int));
	
	
	
	

	/*********************************** Create task ***********************************/
	
  xTaskCreate( vNetworkRxGuardianTask, (const signed char*)"vNetworkRxGuardianTask",
	  256, NULL, 3, NULL );
  xTaskCreate( vNetworkTxGuardianTask, (const signed char*)"vNetworkTxGuardianTask",
	  256, NULL, 3, NULL );
	xTaskCreate( vShellRxGuardianTask, (const signed char*)"vShellRxGuardianTask", 
		256, NULL, 4, NULL );
	xTaskCreate( vShellTxGuardianTask, (const signed char*)"vShellTxGuardianTask", 
		STACK_SIZE_MIN, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vTargetAssignTask, (const signed char*)"Target Assign Task", 
		1024, NULL, 4, NULL );
	xTaskCreate( vBCRxGuardianTask, (const signed char*)"vBCRxGuardianTask", 
		512, NULL, 3, NULL );
	xTaskCreate( vBCTxGuardianTask, (const signed char*)"vBCTxGuardianTask", 
		STACK_SIZE_MIN, NULL, 3, NULL );
//	xTaskCreate( vBCTxTestTask, (const signed char*)"vBCTxTestTask", 
//		STACK_SIZE_MIN, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vRFIDPositionUpdateTask, (const signed char*)"vRFIDPositionUpdateTask", 
			256, NULL, 3, NULL );
	xTaskCreate( vIdleTask, (const signed char*)"vIdleTask", 
			128, NULL, 0, NULL );
	xTaskCreate( vStepmotorDriveTask, (const signed char*)"vStepmotorDriveTask", 
			256, NULL, 3, NULL );
	xTaskCreate( vMovingControlTask, (const signed char*)"vMovingControlTask", 
			300, NULL, 2, NULL );
	xTaskCreate( vPathPlanTask, (const signed char*)"vPathPlanTask", 
			1024, NULL, 1, NULL );
//	xTaskCreate( vCompassReadTask, (const signed char*)"vCompassReadTask", 
//			128, NULL, 3, NULL );
	xTaskCreate( vTracelistMaintainTask, (const signed char*)"vTracelistMaintainTask", 
			128, NULL, 2, NULL );		
//	xTaskCreate( vtime_Test_task, (const signed char*)"vtime_Test_task", 
//			128, NULL, 3, NULL );
			
	xTaskCreate( vDecisionTask, (const signed char*)"vDecisionTask", 
			256, NULL, 2, NULL );
	xTaskCreate( vSeeTargetPosListMaintain, (const signed char*)"vSeeTargetPosListMaintain", 
			256, NULL, 2, NULL );
	xTaskCreate( vGetYawTask, (const signed char*)"vGetYawTask", 
			128, NULL, 4, NULL );		
			
	xTaskCreate( vTraceForecastTask, (const signed char*)"vTraceForecastTask", 
			4096, NULL, 2, NULL );
		printf("Start!\r\n");
	
	vTaskStartScheduler();
}
//******************************************************************************
void go(int Angle, int x, int y)
{
	Position TargetPos;
  if (Angle > 180)
	{
		TargetPos.Angle = Angle - 360;
	}
	else
	{
		TargetPos.Angle = Angle;
	}
  
  TargetPos.x = x;
	TargetPos.y = y;

	xQueueSend(TargetPosQueue, &TargetPos, portMAX_DELAY);
	xSemaphoreTake(MoveComplete, portMAX_DELAY);
}

void setTime(int minutes)
{
	int time;
	time = minutes;
	second_Counter = 0;
	xQueueSend(TimeTestQueue, &time, portMAX_DELAY);
}




QSH_FUN_REG (setTime, "void setTime(int minutes)");
QSH_FUN_REG (go, "void go(int Angle, int x, int y))");

//******************************************************************************
