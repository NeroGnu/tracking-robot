#ifndef __TASK_H
#define __TASK_H

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
#endif