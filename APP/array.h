#ifndef ARRAY_H
#define ARRAY_H
#include <stdio.h>


#define SINGLE 0
#define ROW    1
#define COLUMN 2


void array_inv(float *p,int n);

void swap(float *a,float *b);
float inv(float *p,int n);
void print_array(float *p,int m,int n);
int array_mul(float *p,int m1,int n1,float *q,int m2,int n2,float *out,int m3,int n3);
int array_add(float *p,int m1,int n1,float *q,int m2,int n2,float *out,int m3,int n3);
int array_minu(float *p,int m1,int n1,float *q,int m2,int n2,float *out,int m3,int n3);
int array_T(float *p,int m1,int n1,float *out,int m2,int n2);
int array_set_value(float *p,int m1,int n1,float *q,int m2,int n2,int choose,int num);
int array_get_column(float *p,int m1,int n1,float *out,int m2,int n2,int num);
//int array_generate_rand(float *p,int m1,int n1,int max,int min);
int array_sqrt(float *p,int m1,int n1,float *q,int m2,int n2);
void array_saveas_file(float *p,int m1,int n1,FILE *fp);
int array_equal(float *p,int m1,int n1,float *q,int m2,int n2);
int array_mul_with_num(float *p,int m1,int n1,float num);
void zeros_array(float *p,int m1,int n1);

#endif


