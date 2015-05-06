#ifndef __PUBLIC_H
#define __PUBLIC_H

#include "RNG.h"
#include "Timer2.h"

typedef union 
{
	char c[4];
	float f;
} FloatToChar;

typedef union 
{
	char c[4];
	int i;
} IntToChar;

typedef union 
{
	char c[4];
	unsigned int i;
} UIntToChar;


#endif