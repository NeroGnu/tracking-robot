#include "statPredict.h"
#include "array.h"


int Line_kalman(float *pre_Xkf,						//前一时刻的估计值 （输入）4行1列
				float *pre_P,						//前一时刻的协方差 （输入） 4行4列
				float *now_Z,							//当前时刻的观测值  （输入） 2行1列
				float *pre_R,						//前一时刻的观测误差（输入） 	2行2列
				float *now_Xkf,									// 当前时刻的估计值  （输出） 4行1列
				float *now_R,									//当前时刻的观测误差（输出） 2行2列
				float *now_P,									//当前时刻的协方差（输出）  4行4列
				time_pos *predict, int len_PD					//当前时刻的预测数组（输出）
				)
{

//	float X[4][N] = {0.0f};
	float Q[4][4] = {{0.5,0,0,0},
					 {0,1,0,0},
					 {0,0,0.5,0},
					 {0,0,0,1}
					};
//	float R[2][2] = {{1,0},{0,*1}};
	float F[4][4] = {{1,T,0,0},
					 {0,1,0,0},
					 {0,0,1,T},
					 {0,0,0,1}
					};
	float H[2][4] = {{1,0,0,0},
					 {0,0,1,0}};
	float Xn[4][1];
	float tmp4_1_1[4][1] = {0.0f};
	float tmp4_1_2[4][1] = {0.0f};
	float tmp4_4[4][4] = {0.0f};
	float tmp4_4_1[4][4] = {0.0f};
	float tmp4_4_2[4][4] = {0.0f};
	float tmp2_2[2][2] = {0.0f};
	float tmp4_2[4][2] = {0.0f};
	float tmp4_2_1[4][2] = {0.0f};

	float tmp2_4[2][4] = {0.0f};

	float tmp2_1_1[2][1] = {0.0f};
	float tmp2_1_2[2][1] = {0.0f};

	float K[4][2] = {0};

	float test1[4][1] = {{4},{3},{2},{1}};

	

	float delta_w = 0.001;
	float eye_4[4][4] = {{1,0,0,0},
					  {0,1,0,0},
					  {0,0,1,0},
					  {0,0,0,1}
					  };




	array_mul_with_num(Q[0],4,4,delta_w);
	/*Xn=F*Xkf;*/
	//	array_get_column(Xkf[0],4,N,tmp4_1_1[0],4,1,i-1);
		array_mul(F[0],4,4,pre_Xkf,4,1,Xn[0],4,1);
		
//		printf("Xn=F*Xkf;\r\n");
//		print_array(Xn[0],4,1);
//		printf("\r\n");




		/*P1=F*P0*F'+Q;*/
		array_mul(F[0],4,4,pre_P,4,4,tmp4_4[0],4,4);
		array_T(F[0],4,4,tmp4_4_1[0],4,4);
		array_mul(tmp4_4[0],4,4,tmp4_4_1[0],4,4,tmp4_4_2[0],4,4);
		array_add(tmp4_4_2[0],4,4,Q[0],4,4,tmp4_4_1[0],4,4);
		
		



		/*K=P1*H'*inv(H*P1*H'+R);*/
		array_T(H[0],2,4,tmp4_2_1[0],4,2);
		
//		printf("tmp4_2_1:\r\n");
//		print_array(tmp4_2_1[0],4,2);
//		printf("\r\n");
		
		array_mul(tmp4_4_1[0],4,4,tmp4_2_1[0],4,2,tmp4_2[0],4,2);
		
//		printf("tmp4_2:\r\n");
//		print_array(tmp4_2[0],4,2);
//		printf("\r\n");
		
		

		array_mul(H[0],2,4,tmp4_4_1[0],4,4,tmp2_4[0],2,4);
//		printf("tmp2_4:\r\n");
//		print_array(tmp2_4[0],2,4);
//		printf("\r\n");
		array_mul(tmp2_4[0],2,4,tmp4_2_1[0],4,2,tmp2_2[0],2,2);
//		printf("tmp2_2:\r\n");
//		print_array(tmp2_2[0],2,2);
//		printf("\r\n");
		array_add(tmp2_2[0],2,2,pre_R,2,2,tmp2_2[0],2,2);
//		printf("tmp2_2:\r\n");
//		print_array(tmp2_2[0],2,2);
//		printf("\r\n");
		array_inv(tmp2_2[0],2);
//		printf("tmp2_2:\r\n");
//		print_array(tmp2_2[0],2,2);
//		printf("\r\n");
		array_mul(tmp4_2[0],4,2,tmp2_2[0],2,2,K[0],4,2);

//		printf("K=P1*H'*inv(H*P1*H'+R);\r\n");
//		print_array(K[0],4,2);
//		printf("\r\n");


		/*Xkf(:,i)=Xn+K*(Z(:,i)-H*Xn);*/
		array_mul(H[0],2,4,Xn[0],4,1,tmp2_1_1[0],2,1);
		//array_get_column(Z[0],2,N,tmp2_1_2[0],2,1,i);
		array_minu(now_Z,2,1,tmp2_1_1[0],2,1,tmp2_1_1[0],2,1);
		array_mul(K[0],4,2,tmp2_1_1[0],2,1,tmp4_1_1[0],4,1);
		array_add(Xn[0],4,1,tmp4_1_1[0],4,1,now_Xkf,4,1);
		//array_set_value(Xkf[0],4,N,tmp4_1_1[0],4,1,COLUMN,i);
		
//		printf("Xkf(:,i)=Xn+K*(Z(:,i)-H*Xn);\r\n");
//		print_array(now_Xkf,4,1);
//		printf("\r\n");


		/*P0=(eye(4)-K*H)*P1;*/
		array_mul(K[0],4,2,H[0],2,4,tmp4_4[0],4,4);
		array_minu(eye_4[0],4,4,tmp4_4[0],4,4,tmp4_4[0],4,4);
		array_mul(tmp4_4[0],4,4,tmp4_4_1[0],4,4,now_P,4,4);
		//array_minu(P1[0],4,4,tmp4_4[0],4,4,P0[0],4,4);
		
//		printf("P0=(eye(4)-K*H)*P1;\r\n");
//		print_array(now_P,4,4);
//		printf("\r\n");

	return 1;
}
