#include "compass.h"
#include <stdio.h>
#include "control.h"


#define HMC5883L_Addr 0x3C

void COMP_Init(void)
{
   I2C_Simu_GpioConfig();
}


void COMP_PreRead(void)	 //预测量与真正测量的时间间隔需要在6ms以上
{
	I2C_Simu_Single_Write(HMC5883L_Addr,0x00,0x60);//8次采样取平均
	I2C_Simu_Single_Write(HMC5883L_Addr,0x01,0x00);//最大增益 1370 LSB/Gauss
	I2C_Simu_Single_Write(HMC5883L_Addr,0x02,0x01);//单次测量模式
}


 float COMP_Read(void)
{
	u8 x_h, x_l, y_h, y_l;
  s16 x, y;
	float xb, yb;
	volatile float Angle;
	//vs32 Angle;
	
	x_h=I2C_Simu_Single_Read(HMC5883L_Addr,0x03);
	x_l=I2C_Simu_Single_Read(HMC5883L_Addr,0x04);
	y_h=I2C_Simu_Single_Read(HMC5883L_Addr,0x07);
	y_l=I2C_Simu_Single_Read(HMC5883L_Addr,0x08);
	
	x=(s16)((x_h<<8)|x_l);
  y=(s16)((y_h<<8)|y_l);

  if(x>0x7fff)x-=0xffff;	  
  if(y>0x7fff)y-=0xffff;
//	//以校正过，1号，罗盘重焊过。	
//	printf("%d %d\r\n", x, y);
//	x=x-45;
//	y=y+121;
//	y=y*0.9672;

#ifdef No1
    xb = x + 46.6023;
	  yb = y - 92.9653;
#endif

#ifdef No2
    xb = x + 7.3023;
	  yb = y + 220.4888;
#endif

#ifdef No3
    xb = x - 96.3364;
	  yb = y + 130.9671;
#endif

#ifdef No4
    xb = x - 43.4042;
	  yb = y + 38.8398;
#endif

#ifdef No5
    xb = x - 261.9108;
	  yb = y + 176.4366;
#endif

//NO4:
//   xb = x -  53.0410;
//	 yb = y + 114.3950;
	 
//		NO3:
//	 xb = x - 100.6646;
//	 yb = y + 88.0426;
	
//NO2:
//   xb = x - 32.1635;
//	 yb = y + 213.6851;
	 
//NO1:
//   xb = x + 60.2359;
//	 yb = y + 276.9822;
  //Angle = (u32)(atan2(y,x) * 180.0 / 3.14159265 + 180.0);
	Angle = (atan2(yb,xb) * 180.0 / 3.14159265);// + 180.0);
	return Angle;
}
/*
extern vs32 CurAngle;
int Get_Angle(void)
{
	//COMP_PreRead();
	Delay_1ms(7);
	return CurAngle;
}
*/