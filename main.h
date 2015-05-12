#ifndef __MAIN_H
#define __MAIN_H

#include "f4_head.h"
#include "os.h"
#include "drivers.h"
#include "app.h"
#include "Public.h"

#define STACK_SIZE_MIN	128	/* usStackDepth	- the stack size DEFINED IN WORDS.*/


/*********************Task List**************************/

void vShellRxGuardianTask(void *pvParameters);
void vShellTxGuardianTask(void *pvParameters);

void vNetworkRxGuardianTask(void *pvParameters);
void vNetworkTxGuardianTask(void *pvParameters);

void vTargetAssignTask(void *pvParameters);

void vBCRxGuardianTask(void *pvParameters);
void vBCTxGuardianTask(void *pvParameters);
//void vBCTxTestTask(void *pvParameters);

void vStepmotorDriveTask(void *pvParameters);
void vMovingControlTask(void *pvParameters);
void vPathPlanTask(void *pvParameters);

void vRFIDPositionUpdateTask(void *pvParameters);
void vCompassReadTask(void *pvParameters);

void vTracelistMaintainTask(void *pvParameters);

void vtime_Test_task(void *pvParameters);

void vCycleRecoTask(void *pvParameters);

void vIdleTask(void *pvParameters);

void vDecisionTask(void *pvParameters);

void vSeeTargetPosListMaintain(void *pvParameters);

void vGetYawTask(void *pvParameters);

/**********extern from main***************/
/* Uart IO pool resource semaphore. */
extern xSemaphoreHandle OutBufferMutex, OutPoolNum, InPoolEmpty, InPoolCounterMutex, InPoolFull;
extern xSemaphoreHandle OutPoolEmpty, OutPoolCounterMutex, OutPoolFull;

/* Network send and receive pool resource semaphore. */
extern xSemaphoreHandle GetPacketPoolCounterMutex, GetPacketPoolEmpty, GetPacketPoolFull;
extern xSemaphoreHandle SendPacketPoolCounterMutex, SendPacketPoolEmpty, SendPacketPoolFull;

/* Target informaion queue semaphore. */
extern xSemaphoreHandle TargetInformationCounterMutex, TargetInformationEmpty, TargetInformationFull;
extern xSemaphoreHandle SelfTargetInformationMutex;

/* Board communication recieve and send synchronous semaphore. */
extern xSemaphoreHandle BCRxSyn, BCTxSyn;

/* Self X coordinate semaphore.*/
extern xSemaphoreHandle SelfXCoordinateSyn;

/* Tag synchronization.*/
extern xSemaphoreHandle TagSyn;

extern xSemaphoreHandle CorrectAngleSyn;

/* Move complete synchronization.*/
extern xSemaphoreHandle MoveComplete;

/* Forward stop enable */
extern xSemaphoreHandle ForwardStopEnable;

/* Move stop enable */
extern xSemaphoreHandle MoveStopEnable;

extern xSemaphoreHandle ComunicationWithBoardSyn;

/* AHRS data synchronization*/
extern xSemaphoreHandle AHRS_Syn;

/*find the target*/
extern xSemaphoreHandle FoundTargetSyn;

/* System output semaphore */
extern xSemaphoreHandle OutputMutex;

//****************************************************

/* Move command queue.*/
extern xQueueHandle MoveCmdQueue;

/* Move result queue.*/
extern xQueueHandle MoveRsultQueue;

/* Target Position queue.*/
extern xQueueHandle TargetPosQueue;

/* Arduino feedback queue.*/
extern xQueueHandle ArduinoFeedbackQueue;

/* Arduino query queue.*/
extern xQueueHandle ArduinoQueryQueue;

/* Position update queue.*/
extern xQueueHandle PositionUpdatekQueue;

/* Board communication Tx queue. */
extern xQueueHandle BCTxQueue;

/* Trace node queue. */
extern xQueueHandle TraceNodeQueue;

/* CC2500 Rx Queue. */
extern xQueueHandle CC2500RxQueue;

/* CC2500 Tx Queue. */
extern xQueueHandle CC2500TxQueue;

/* TargetATPos Queue. */
extern xQueueHandle TargetATPosQueue;


extern xQueueHandle TimeTestQueue;

/***************variable******************/

extern char SelfTargetInformation[TARGETINFORMATIONLENGTH];
extern FloatToChar SelfXCoordinate[TARGETNUM];
extern FloatToChar SelfXDistance[TARGETNUM];

extern volatile float CurAngle;
extern float Globaldis;

extern volatile int goflag, ComfirAlreadyflag;
extern int count_change_angle, count_change_pos,isFindOver;

extern Procedure GlobleCurProcedure;
extern Position TargetPositionList[TARGETNUM];
extern TargetATPos SeeTargetPosList[TARGETNUM];
extern char SelfConfluentEfficiency[TARGETNUM + 1];


extern Position Cache1[TARGETNUM], Cache2[TARGETNUM];
extern int Cache1Time, Cache2Time;

#define SystemOut(fmt,args...) \
xSemaphoreTake(OutputMutex,portMAX_DELAY);\
printf(fmt,##args);\
xSemaphoreGive(OutputMutex)

#endif