#ifndef __SHELL_H
#define __SHELL_H

#include "stm32f4xx.h"
#include <stdio.h>

#define INBUFFERSIZE 50
#define INBUFFERNUM  3

#define OUTBUFFERSIZE 50
#define OUTBUFFERNUM  15

typedef struct
{
	int front;
	int rear;
	int length;
	int width;
	char *ppool;
}IO_Pool;

extern char rxBuffer[INBUFFERSIZE];
extern char InBuffer[INBUFFERNUM][INBUFFERSIZE];
extern IO_Pool InPool;

extern char OutBuffer[OUTBUFFERNUM][OUTBUFFERSIZE];
extern IO_Pool OutPool;

void Com2Init(void);
char *GetBuffer(IO_Pool*);
char *FreeBuffer(IO_Pool *pool);

#endif

