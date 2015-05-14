
#include <stdlib.h>

#include <math.h>

#include "array.h"
#include "f4_head.h"



/*
//如果有问题   首先检查这里
void array_inv(float *p,int n)
{
	arm_matrix_instance_f32 src;
	arm_matrix_instance_f32 des;
	float32_t *q;
    q = (float32_t *)malloc(n*n*sizeof(float32_t));

	arm_mat_init_f32(&src,n,n,(float32_t *)p);
	arm_mat_init_f32(&des,n,n,(float32_t *)q);

	arm_mat_inverse_f32(&src,&des);

	free(p);

	p = des.pData;
}
*/

void swap(float *a,float *b)
{
	float c;
	c=*a; *a=*b; *b=c;
}

void array_inv(float *p,int n)
{
	int *is,*js,i,j,k,m,f = 1;
	float temp,fMax,fDet=1.0f;
	/*
	printf("the original array is:\n");
	for(i = 0; i < n; i ++)
	{
		putchar('\n');
		for(j = 0; j < n; j++)
			printf("%f\t",*(p+i*n+j));
	}
	puts("\n\n\n");

	*/
	is = (int *)malloc(n*sizeof(int));
	js = (int *)malloc(n*sizeof(int));
	for(k = 0; k < n; k++)
	{
		fMax = 0.0f;
		for(i = k; i < n; i++)
			for(j = k; j < n;j++)
			{
				temp = fabs(*(p+i*n+j));
				if(temp > fMax)
				{
					fMax = temp;
					is[k] = i;
					js[k] = j;

				}
			}
		if((fMax+1.0 ) == 1.0 )
		{
			free(is);
			free(js);
			printf("NO INV");
			//return 0;
		}


		if((i = is[k]) != k)
		{
			f = -f;
			for(j = 0; j < n; j++)
				swap((p+k*n+j),(p+i*n+j));
		}

		if((j = js[k]) != k)
		{
			f = -f;
			for(i = 0; i < n; i++)
				swap((p+i*n+k),(p+i*n+j));
		}

		fDet *= (*(p+k*n+k));
		(*(p+k*n+k)) = 1.0/(*(p+k*n+k));

		for(j = 0; j < n; j++)
			if(j != k)
				(*(p+k*n+j)) *= (*(p+k*n+k));

		for(i = 0; i < n; i++)
		{
			if(i != k)
			for(j = 0; j < n; j++)
				if(j != k)
					*(p+i*n+j) = *(p+i*n+j) - (*(p+i*n+k)) * (*(p+k*n+j));
		}

		for(i = 0; i < n; i++)
		{
			if(i != k)
				*(p+i*n+k) *= -(*(p+k*n+k));
		}
	}

	for(k = n-1; k >= 0; k--)
	{
		if((i = is[k]) != k)
			for(j = 0; j < n; j++)
				swap((p+j*n+k),(p+j*n+i));

		if((j = js[k]) != k)
			for(i = 0; i < n; i++)
				swap((p+k*n+i),(p+j*n+i));
	}

	free(is);
	free(js);
	//return f*fDet;




}


void print_array(float *p,int m,int n)
{
	int i,j;
	for(i = 0; i < m; i++)
	{
		printf("\n");
		for(j = 0; j < n; j++)
			printf("%f \t",*(p+i*n +j));
	}
}

int array_mul_with_num(float *p,int m1,int n1,float num)
{
	arm_matrix_instance_f32 src;


	arm_mat_init_f32(&src,m1,n1,(float32_t *)p);
	arm_mat_scale_f32(&src,num,&src);

	p = src.pData;
	return 1;
}

int array_mul(float *p,int m1,int n1,float *q,int m2,int n2,float *out,int m3,int n3)
{

	int i,j,k;
	float tmp;
	
  arm_matrix_instance_f32 src;
	arm_matrix_instance_f32 des;
	arm_matrix_instance_f32 res;

	arm_mat_init_f32(&src,m1,n1,(float32_t *)p);
	arm_mat_init_f32(&des,m2,n2,(float32_t *)q);
	arm_mat_init_f32(&res,m3,n3,(float32_t *)out);

	arm_mat_mult_f32(&src,&des,&res);
	
	if(n1 != m2 || m3 != m1 || n3 != n2)
	{
		printf("input array erro,please check out");
		return 0;
	}


    out = res.pData;


	return 1;

}


int array_add(float *p,int m1,int n1,float *q,int m2,int n2,float *out,int m3,int n3)
{

	int i,j;
	arm_matrix_instance_f32 src;
	arm_matrix_instance_f32 des;
	arm_matrix_instance_f32 res;
	if(n1 != n2 || m2 != m1 || n3 != n2 || m3 != m1)
	{
		printf("input array erro,please check out");
		return 0;
	}

	

	arm_mat_init_f32(&src,m1,n1,(float32_t *)p);
	arm_mat_init_f32(&des,m2,n2,(float32_t *)q);
	arm_mat_init_f32(&res,m3,n3,(float32_t *)out);

	arm_mat_add_f32(&src,&des,&res);

    out = res.pData;
	return 1;

}


int array_minu(float *p,int m1,int n1,float *q,int m2,int n2,float *out,int m3,int n3)
{

	int i,j;
	float tmp;
	arm_matrix_instance_f32 src;
	arm_matrix_instance_f32 des;
	arm_matrix_instance_f32 res;
	if(n1 != n2 || m2 != m1 || n3 != n2 || m3 != m1)
	{
		printf("input array erro,please check out");
		return 0;
	}

	
	
	arm_mat_init_f32(&src,m1,n1,(float32_t *)p);
	arm_mat_init_f32(&des,m2,n2,(float32_t *)q);
	arm_mat_init_f32(&res,m3,n3,(float32_t *)out);

	arm_mat_sub_f32(&src,&des,&res);

    out = res.pData;
	return 1;

}


int array_T(float *p,int m1,int n1,float *out,int m2,int n2)
{

	int i,j,k=0;
	arm_matrix_instance_f32 src;
	arm_matrix_instance_f32 des;
	if(m1 != n2 || n1 != m2)
	{
		printf("input array erro,please check out");
		return 0;
	}

	


	arm_mat_init_f32(&src,m1,n1,(float32_t *)p);
	arm_mat_init_f32(&des,m2,n2,(float32_t *)out);

	arm_mat_trans_f32(&src,&des);

	out = des.pData;
	return 1;
}
//将矩阵p的
//如果choose =  SINGLE 则将矩阵p的 第m2行n2列 的数据设为num
//如果choose =  ROW 则将矩阵p的 第num行 的数据设为q[1][n2]
//如果choose =  COLUMN 则将矩阵p的 第num列 的数据设为q[m2][1]
//
//
//
//
int array_set_value(float *p,int m1,int n1,float *q,int m2,int n2,int choose,int num)
{

	int i;
	switch(choose)
	{
		case SINGLE:
				if(m2 >= m1 || m2 < 0||n2 >= n1 || n2 < 0)
				{
					printf("input array erro,please check out");
					return 0;
				}
				*(p+m2*n1+n2) = num;
				break;
		case ROW:
				if(m2 != 1 ||n2 != n1)
				{
					printf("input array erro,please check out");
					return 0;
				}
				for(i = 0; i < n1; i++)
					*(p+num*n1+i) = *(q+i);
				break;
		case COLUMN:
				if(m2 != m1 ||n2 != 1)
				{
					printf("input array erro,please check out");
					return 0;
				}

				for(i = 0; i < m1; i++)
					*(p+i*n1+num) = *(q+i);
				break;
		default:
			break;
	}
	return 1;
}


//从二维数组p[m1][n1] 取出第num列 ，结果放到out[m2][n2]中
int array_get_column(float *p,int m1,int n1,
					float *out,int m2,int n2,int num)
{
	int i;
	if(m2 != m1 ||n2 != 1)
	{
		printf("input array erro,please check out");
		return 0;
	}
	
	for(i = 0; i < m1; i++)
		*(out+i) = *(p+i*n1+num);
	//printf("array_get_column\r\n");	
		
	return 1;
}


/*
int array_generate_rand(float *p,int m1,int n1,int max,int min)
{
	int i,j;

	for(i = 0; i < m1; i++)
	{
		srand( (unsigned int)time(0) );
		for(j = 0; j < n1; j++)
		{
			srand( (unsigned int)time(0) + (i+1)*(j+1)*100);
			*(p + i*n1 + j) = (float)(rand() % ((max - min)*1000) + min*1000) / 1000;
		}
	}
	return 1;
}
*/

//将p[m1][n1]中的元素开方运算
int array_sqrt(float *p,int m1,int n1,float *q,int m2,int n2)
{
	int i,j;
	if(m1 != m2 || n1 != n2)
	{
		printf("input array erro,please check out");
		return 0;
	}
	for(i = 0; i < m1; i++)
	{
		for(j = 0; j < n1; j++)
			*(q + i*n1 + j) = sqrt(*(p + i*n1 + j));
	}
	return 1;
}

//把矩阵 p[m1][n1]输出到文件中去
void array_saveas_file(float *p,int m1,int n1,FILE *fp)
{
	int i,j;

	for(i = 0; i < m1; i++)
	{
		//printf("\r\n");
		fprintf(fp, "\n");
		for(j = 0; j < n1; j++)
			fprintf(fp, "%f\t", *(p+i*n1 +j));
	}

}

//将p[m1][n1]复制到q[m2][n2]中
int array_equal(float *p,int m1,int n1,float *q,int m2,int n2)
{
	int i,j;
	if(m1 != m2 || n1 != n2)
	{
		printf("input array erro,please check out");
		return 0;
	}
	for(i = 0; i < m1; i++)
	{
		for(j = 0; j < n1; j++)
			*(q + i*n1 + j) = *(p + i*n1 + j);
	}
	return 1;
}

void zeros_array(float *p,int m1,int n1)
{
    int i,j;
    for(i = 0; i < m1; i++)
	{
		for(j = 0; j < n1; j++)
			*(p + i*n1 + j) = 0;
	}
}

