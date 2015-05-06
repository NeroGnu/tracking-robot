#include "target_assign.h"
#include "network.h"

#include <stdio.h>

char TargetInformationBuffer[TARGETINFORMATIONQUEUE][TARGETINFORMATIONLENGTH];
IO_Pool TargetInformationPool = {0, 0, TARGETINFORMATIONQUEUE, TARGETINFORMATIONLENGTH, &TargetInformationBuffer[0][0]};

u8 OrderIndex[LIMITNODENUM + 2] = {0};
int AssignResult[MAXNUM] = {0};

void Init_Matrix(matrix_u8 *matrix, u8 row, u8 col, s8 value)
{
	matrix->numRows = row;
	matrix->numCols = col;
	arm_fill_q7(0, &matrix->Data[0][0], M*N);
	while (row > 0)
	{
		row--;
		arm_fill_q7(value, &matrix->Data[row][0], col);
	}
}

void Indexing(u8 index[], u8 list[], int length)
{
	int i, counter;
	counter = 0;
	for (i = 0; i < length; i++)
	{
		if (0 != list[i])
		{
			index[i] = counter;
			counter++;
		}
	}
}

void col_max_q7(s8 *pSrc, u32 blockSize, u32 width, s8 *pResult, u32 *pIndex)
{
	int i;
	*pResult = *pSrc;
	for (i = 0; i < blockSize; i++)
	{
		*pResult = *pSrc > *pResult ? *pSrc : *pResult;
		*pIndex = i;
		pSrc = pSrc + width;
	}
}

void col_min_q7(s8 *pSrc, u32 blockSize, u32 width, s8 *temp)
{
	int i;
	*temp = *pSrc;
	for (i = 0; i < blockSize; i++)
	{
		*temp = *pSrc < *temp ? *pSrc : *temp;
		pSrc = pSrc + width;
	}
}

void col_fill_q7(s8 *pSrc, s8 value, u32 width, u32 blockSize)
{
	int i;
	for (i = 0; i < blockSize; i++)
	{
		*pSrc = value;
		pSrc = pSrc + width;
	}
}

int matrix_sum(matrix_u8 *matrix)
{
	int sum = 0, i, j;
	for (i = 0; i < matrix->numRows; i++)
	{
		for (j = 0; j < matrix->numCols; j++)
		{
			sum = sum + matrix->Data[i][j];
		}
	}
	return sum;
}

void matrix_dot_multiply(matrix_u8 *matrix_a, matrix_u8 *matrix_b, matrix_u8 *matrix_c)
{
	int i, j;
	for (i = 0; i < matrix_a->numRows; i++)
	{
		for (j = 0; j < matrix_a->numCols; j++)
		{
			matrix_c->Data[i][j] = matrix_a->Data[i][j] * matrix_b->Data[i][j];
		}
	}
}

void IndexingMax(INDEX *index, matrix_u8 *matrix)
{
	int i;
	
	arm_max_q7(&matrix->Data[0][0], matrix->numCols, &index->row_value[0], &index->row_index[0]);
	index->value = index->row_value[0];
	index->i = 0;
	index->j = index->row_index[0];
	
	for (i = 1; i < matrix->numRows; i++)
	{
		arm_max_q7(&matrix->Data[i][0], matrix->numCols, &index->row_value[i], &index->row_index[i]);
		if (index->row_value[i] > index->value)
		{
			index->value = index->row_value[i];
			index->i = i;
			index->j = index->row_index[i];
		}
	}
	
	for (i = 0; i < matrix->numCols; i++)
	{
		col_max_q7(&matrix->Data[i][0], matrix->numRows, N, &index->col_value[i], &index->col_index[i]);
	}
}

int TargetAssign(matrix_u8 matrix, matrix_u8 *ans)
{
	matrix_u8 matrix_b = matrix, anti_ans;
	INDEX max_index;
	int i, j, round;
	
	Init_Matrix(ans, matrix.numRows, matrix.numCols, 0);
	Init_Matrix(&anti_ans, matrix.numRows, matrix.numCols, 1);
	
	round = matrix.numRows / matrix.numCols;
	for (j = 0; j < round; j++)
	{
		for(i = j * matrix.numCols; i < (j + 1) * matrix.numCols; i++)
		{
			IndexingMax(&max_index, &matrix);
		  if (0 != matrix.Data[max_index.i][max_index.j])
			{
				ans->Data[max_index.i][max_index.j] = 1;
				//行不能攻击多个木标，所以整行置0；
				arm_fill_q7(0, &anti_ans.Data[max_index.i][0], anti_ans.numCols);
				//anti_ans.Data[max_index.i][max_index.j] = 0;
			
				arm_fill_q7(0, &matrix.Data[max_index.i][0], matrix.numCols);
			
				col_fill_q7(&matrix.Data[0][max_index.j], 0, N, matrix.numRows);
			}		  
		}
		
		matrix = matrix_b;
		matrix_dot_multiply(&matrix, &anti_ans, &matrix);
	}
	
	round = matrix.numRows % matrix.numCols;
	for (i = 0; i < round; i++)
	{
		IndexingMax(&max_index, &matrix);
		if (0 != matrix.Data[max_index.i][max_index.j])
		{
			ans->Data[max_index.i][max_index.j] = 1;
			//行不能攻击多个木标，所以整行置0；
			arm_fill_q7(0, &anti_ans.Data[max_index.i][0], anti_ans.numCols);
			//anti_ans.Data[max_index.i][max_index.j] = 0;
		
			arm_fill_q7(0, &matrix.Data[max_index.i][0], matrix.numCols);
		
			col_fill_q7(&matrix.Data[0][max_index.j], 0, N, matrix.numRows);
		}		  
	}
	
	matrix_dot_multiply(&matrix_b, ans, &matrix_b);
	return matrix_sum(&matrix_b);
}
