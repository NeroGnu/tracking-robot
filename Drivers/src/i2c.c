#include "i2c.h"

//#include "stm32f10x.h"
#define SCL_AHB       RCC_AHB1Periph_GPIOE
#define SDA_AHB       RCC_AHB1Periph_GPIOE
#define SCL_GPIO      GPIOE
#define SDA_GPIO      GPIOE
#define SCL_PIN       GPIO_Pin_0
#define SDA_PIN       GPIO_Pin_1

#define SCL_H         SCL_GPIO->BSRRL = SCL_PIN
#define SCL_L         SCL_GPIO->BSRRH = SCL_PIN 
   
#define SDA_H         SDA_GPIO->BSRRL = SDA_PIN
#define SDA_L         SDA_GPIO->BSRRH = SDA_PIN

#define SCL_read      (SCL_GPIO->IDR  & SCL_PIN)
#define SDA_read      (SDA_GPIO->IDR  & SDA_PIN)

static void I2C_delay(void)
{		
   u8 i=30; //��������Ż��ٶ�	����������͵�5����д��

   while(i--);
}
/*
static void delay5ms(void)
{		
   s32 i=5000; 
    
   while(i--); 
}
*/
static bool I2C_Simu_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read) return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;
	I2C_delay();
	return TRUE;
}

static void I2C_Simu_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 

static void I2C_Simu_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 

static bool I2C_Simu_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
	  I2C_delay();
      return FALSE;
	}
	SCL_L;
	I2C_delay();
	return TRUE;
}

static void I2C_Simu_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}  

static u8 I2C_Simu_RadeByte(void)  //���ݴӸ�λ����λ
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
} 

void I2C_Simu_GpioConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_AHB1PeriphClockCmd(SCL_AHB, ENABLE);
	RCC_AHB1PeriphClockCmd(SDA_AHB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SCL_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SDA_GPIO, &GPIO_InitStructure);
	
	/*
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	*/
	
}

bool I2C_Simu_Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data)		
{
  	if(!I2C_Simu_Start())	return FALSE;
    I2C_Simu_SendByte(SlaveAddress);
    if(!I2C_Simu_WaitAck()){I2C_Simu_Stop(); return FALSE;}
    I2C_Simu_SendByte(REG_Address );   //���õ���ʼ��ַ      
    I2C_Simu_WaitAck();	
    I2C_Simu_SendByte(REG_data);
    I2C_Simu_WaitAck();   
    I2C_Simu_Stop(); 
    //delay5ms();
    return TRUE;
}


u8 I2C_Simu_Single_Read(u8 SlaveAddress,u8 REG_Address)
{   
	u8 REG_data;  
   	
	if(!I2C_Simu_Start())	return FALSE;
    I2C_Simu_SendByte(SlaveAddress); 
    if(!I2C_Simu_WaitAck()){I2C_Simu_Stop(); return FALSE;}
    I2C_Simu_SendByte((u8) REG_Address);   //���õ���ʼ��ַ      
    I2C_Simu_WaitAck();
    I2C_Simu_Start();
    I2C_Simu_SendByte(SlaveAddress+1);
    I2C_Simu_WaitAck();

	REG_data= I2C_Simu_RadeByte();
    I2C_Simu_NoAck();
    I2C_Simu_Stop();
    
	return REG_data;
}
