#ifndef __NETWORK_H
#define __NETWORK_H
#include "stm32f4xx.h"
#include "shell.h"
#include "cc2500.h"
#include "control.h"

#define LIMITNODENUM 254
#define MAXNUM       8       //2~254
//#define SELFADDRESS  SELFID    //0x01~0xFE
#define GETPACKETBUFFERNUM 30
#define SENDPACKETBUFFERNUM 5
#define REPEATTIME 3

#define DES_NUM 0
#define SRC_NUM 1
#define PRC_NUM 2
#define LEN_NUM 3
#define LIS_START 4

#define DISCOVERY_MESSAGE 0x00
#define DISCOVERY_ACCOMPLISHED 0x01
#define TARGETINFORMATION 0x02
#define GYROPIDCMD 0x03
#define GYROPIDCMDACK 0x04

#define ISIDLE       GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)

void NETWORK_IRQ_HOOK();

void NetworkConnecting();
void SelfSend();

bool CC2500_PacketCheck();
void PreDiscovery(void *pvParameters);
void PreDiscoveryAC(void *pvParameters);
//void PreDiscoveryAK(void *pvParameters);
void Discovery(void *pvParameters);
void DiscoveryAC(void *pvParameters);
void TargetInformaion(void *pvParameters);
void TraceExchange(void *pvParameters);
void SeeTargetPosExchange(void *pvParameters);
void ConfluentEfficiencyExchange(void *pvParameters);

/* Global variables */
extern void (*Process[])();
extern IO_Pool GetPacketPool;
extern u8 NbrList[], sPacketbuf[], NodeFlag[];
extern vu8 NbrCount, DiscoveryRequest, OSStart;
extern vu32 ReceiveTime;
extern IO_Pool SendPacketPool;
#endif
