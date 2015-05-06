#ifndef __TARGET_ASSIGN_H__
#define __TARGET_ASSIGN_H__

#include "stm32f4xx.h"
#include "arm_math.h"
#include "shell.h"

#define M 16  // MAX_NUM
#define N 16  // MAX_TARGET

#define TARGETNUM (6)

#define TARGETINFORMATIONQUEUE (5)
#define TARGETINFORMATIONLENGTH (1+TARGETNUM)

typedef struct
{
	u8 numRows;
	u8 numCols;
	s8 Data[M][N];
} matrix_u8;

typedef struct
{	
	u32 row_index[M];
	s8 row_value[M];
	u32 col_index[N];
	s8 col_value[N];
	u32 i;
	u32 j;
	s8 value;
} INDEX;

int TargetAssign(matrix_u8 matrix, matrix_u8 *ans);
void Init_Matrix(matrix_u8 *matrix, u8 row, u8 col, s8 value);
void Indexing(u8 index[], u8 list[], int length);

extern char TargetInformationBuffer[][TARGETINFORMATIONLENGTH];
extern IO_Pool TargetInformationPool;
extern u8 OrderIndex[];
extern int AssignResult[];
#endif

