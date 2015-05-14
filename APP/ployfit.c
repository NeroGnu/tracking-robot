/*==================polyfit(n,x,y,poly_n,a)===================*/
/*=======���y=a0+a1*x+a2*x^2+����+apoly_n*x^poly_n========*/
/*=====n�����ݸ��� xy������ֵ poly_n�Ƕ���ʽ������======*/
/*===����a0,a1,a2,����a[poly_n]��ϵ����������һ�������=====*/

#include "ployfit.h"
#include <stdlib.h>

void polyfit(int n,float x[],float y[],int poly_n,float a[])
{
	int i,j;
	float *tempx,*tempy,*sumxx,*sumxy,*ata;

	tempx=calloc(n,sizeof(float));
	sumxx=calloc(poly_n*2+1,sizeof(float));
	tempy=calloc(n,sizeof(float));
	sumxy=calloc(poly_n+1,sizeof(float));
	ata=calloc((poly_n+1)*(poly_n+1),sizeof(float));
	for (i=0;i<n;i++)
	{
		tempx[i]=1;
		tempy[i]=y[i];
	}
	for (i=0;i<2*poly_n+1;i++)
	for (sumxx[i]=0,j=0;j<n;j++)
	{
		sumxx[i]+=tempx[j];
		tempx[j]*=x[j];
	}
	for (i=0;i<poly_n+1;i++)
	for (sumxy[i]=0,j=0;j<n;j++)
	{
		sumxy[i]+=tempy[j];
		tempy[j]*=x[j];
	}
	for (i=0;i<poly_n+1;i++)
	for (j=0;j<poly_n+1;j++)
		ata[i*(poly_n+1)+j]=sumxx[i+j];
	gauss_solve(poly_n+1,ata,a,sumxy);
	free(tempx);
	free(sumxx);
	free(tempy);
	free(sumxy);
	free(ata);
}

float my_fabs(float num)
{
	if(num > 0)
		return num;
	else
		return 0-num;
	 
}


void gauss_solve(int n,float A[],float x[],float b[])
{
	int i,j,k,r;
	float max;
	for (k=0;k<n-1;k++)
	{
		max=my_fabs(A[k*n+k]); /*find maxmum*/
		r=k;
		for (i=k+1;i<n-1;i++)
		if (max<my_fabs(A[i*n+i]))
		{
			max=my_fabs(A[i*n+i]);
			r=i;
		}
		if (r!=k)
		for (i=0;i<n;i++) /*change array:A[k]&A[r] */
		{
			max=A[k*n+i];
			A[k*n+i]=A[r*n+i];
			A[r*n+i]=max;
		}
		max=b[k]; /*change array:b[k]&b[r] */
		b[k]=b[r];
		b[r]=max;
		for (i=k+1;i<n;i++)
		{
			for (j=k+1;j<n;j++)
				A[i*n+j]-=A[i*n+k]*A[k*n+j]/A[k*n+k];
			b[i]-=A[i*n+k]*b[k]/A[k*n+k];
		}
	} 
	for (i=n-1;i>=0;x[i]/=A[i*n+i],i--)
	for (j=i+1,x[i]=b[i];j<n;j++)
		x[i]-=A[i*n+j]*x[j];
}




