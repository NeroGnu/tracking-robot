#include "cc2500.h"


#ifdef EX_DELAY

#define Delay_ms Delay_1ms

#endif

//PA��
static const u8 paTable_CC2500[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};  //PA value

u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
{
	while((SPIx->SR&SPI_I2S_FLAG_TXE)==RESET);		//�ȴ���������	  
	SPIx->DR=Byte;	 	//����һ��byte   
	while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);//�ȴ�������һ��byte  
	return SPIx->DR;          	     //�����յ�������			
} 

static void CC2500_WriteByte(u8 Data)
{
	SPI_WriteByte(SPI2,Data);
}

static u8 CC2500_ReadByte(void)
{
	return SPI_WriteByte(SPI2,0xff);
}


//����GDIO0�Ŷ�Ӧ���ж�
void CC2500_InterruptEnable(bool Enable)
{
	if(Enable)
		EXTI->IMR |= EXTI_Line4;//�����ж�
	else
		EXTI->IMR &=~EXTI_Line4;//�ر��ж�
}

#ifndef EX_DELAY
//��ʱ����
void Delay_ms(u32 nCount)
{
	int i;
	for(; nCount != 0; nCount--)
		for(i=40; i != 0; i--);
}

#endif

//��CC2500��������
void SpiStrobe(u8 value)
{
	CC2500_Select();
	CC2500_WriteByte(value);
	CC2500_NoSelect();
}

//CC2500�ϵ��ĳ�ʼ��
static void POWER_UP_RESET_CCxxx0(void)
{
	SpiStrobe(CCxxx0_SIDLE);
	Delay_ms(20);
	CC2500_Select();
	Delay_ms(20);
	CC2500_NoSelect();
	Delay_ms(40);
	SpiStrobe(CCxxx0_SRES);
	Delay_ms(20);
}

//д�Ĵ�����ֵ
void CC2500_WriteReg(u8 addr, u8 value)
{
	CC2500_Select();
	addr &= 0x7F;
	CC2500_WriteByte(addr);
	CC2500_WriteByte(value);
	CC2500_NoSelect();
}

//�����õ���IO�� B5->GDO2 B4->GDO0 B12->CS
static void CC2500_GPIO_Config(void)
{
  GPIO_InitTypeDef 	 GPIO_InitStructure;
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Connect EXTI Line4 to PB4 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12);  
	
  //GDO0->B4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  //GDO2->B5
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


static void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
}

/*
//ԭʼ����CC2500
static void CC2500_WriteRfSettings()
{
	CC2500_WriteReg(CCxxx0_FSCTRL1,	0x09);	// 0x0c
	CC2500_WriteReg(CCxxx0_FSCTRL0,	0x00);
	CC2500_WriteReg(CCxxx0_FREQ2,	0x5D);
	CC2500_WriteReg(CCxxx0_FREQ1,	0x93);
	CC2500_WriteReg(CCxxx0_FREQ0,	0xB1);
	CC2500_WriteReg(CCxxx0_MDMCFG4,	0x2D); //0x78-10k   0x86
	CC2500_WriteReg(CCxxx0_MDMCFG3,	0x3B); //0x93-10k   0x83
	CC2500_WriteReg(CCxxx0_MDMCFG2,	0x73); //0x03-10k  0x03
	CC2500_WriteReg(CCxxx0_MDMCFG1,	0xA2); //0x22-10k	 0x22						
	CC2500_WriteReg(CCxxx0_MDMCFG0,	0xF8); //0xf8        0xf8
	CC2500_WriteReg(CCxxx0_CHANNR,	0x00);
	CC2500_WriteReg(CCxxx0_DEVIATN,	0x01);	//0x44	// 00
	CC2500_WriteReg(CCxxx0_FREND1,	0x56);	// 0xB6   56������ʼ���
	CC2500_WriteReg(CCxxx0_FREND0,	0x10);
	CC2500_WriteReg(CCxxx0_MCSM1,	0x30);	// 0X00>NO CCA; 0X30>CCA modify by cy ����cca���޻����ڽ������ݰ��򱨾�
	CC2500_WriteReg(CCxxx0_MCSM0,	0x28);  //0x18
	CC2500_WriteReg(CCxxx0_FOCCFG,	0x15);	// 0x1D   Ƶ��ƫ�Ʋ���
	CC2500_WriteReg(CCxxx0_BSCFG,	0x6C);	// 0x1c   λͬ������
	CC2500_WriteReg(CCxxx0_AGCCTRL2,0x07);	// 0xc3    �������
	CC2500_WriteReg(CCxxx0_AGCCTRL1,0x38);	//modify by cy cca����ȡ��ǰRSSI+14db
	CC2500_WriteReg(CCxxx0_AGCCTRL0,0x91);	// 0xb2
	CC2500_WriteReg(CCxxx0_FSCAL3,	0xEA);
	CC2500_WriteReg(CCxxx0_FSCAL2,	0x0A);  //0x0a - 250k  0x06 --10k,0x08-2.4k
	CC2500_WriteReg(CCxxx0_FSCAL1,	0x00);	// ����Ƶ��ͬ��У׼
	CC2500_WriteReg(CCxxx0_FSCAL0,	0x11);
	CC2500_WriteReg(CCxxx0_FSTEST,	0x59);
	CC2500_WriteReg(CCxxx0_TEST2,	0x8F);	// 0x88
	CC2500_WriteReg(CCxxx0_TEST1,	0x21);	// 0x31
	CC2500_WriteReg(CCxxx0_TEST0,	0x0B);
	CC2500_WriteReg(CCxxx0_IOCFG2,	0x09);	//29 // CCA=0x09:1=free modify by cy����gdio2����ŵ��Ƿ�æ(æ0��1)
	CC2500_WriteReg(CCxxx0_IOCFG0,	0x01);	//0x01//0x07
	CC2500_WriteReg(CCxxx0_FIFOTHR,0x07);	
	CC2500_WriteReg(CCxxx0_PKTCTRL1,0x07);	//0x07//0x02//04
	CC2500_WriteReg(CCxxx0_PKTCTRL0,0x04);	//0x41 whiter+val	 //crc on 05						
	CC2500_WriteReg(CCxxx0_ADDR,	0x02);
	CC2500_WriteReg(CCxxx0_PKTLEN,	PACKET_LEN);
}
*/

 //test MDMCFG4~0����smartRF���ã�spi8��Ƶ��
static void CC2500_WriteRfSettings()
{
	CC2500_WriteReg(CCxxx0_FSCTRL1,	0x09);	// 0x0c
	CC2500_WriteReg(CCxxx0_FSCTRL0,	0x00);
	CC2500_WriteReg(CCxxx0_FREQ2,	0x5D);
	CC2500_WriteReg(CCxxx0_FREQ1,	0x93);
	CC2500_WriteReg(CCxxx0_FREQ0,	0xB1);
	CC2500_WriteReg(CCxxx0_MDMCFG4,	0x0E); //0x78-10k   0x86
	CC2500_WriteReg(CCxxx0_MDMCFG3,	0x3B); //0x93-10k   0x83
	CC2500_WriteReg(CCxxx0_MDMCFG2,	0x73); //0x03-10k  0x03
	CC2500_WriteReg(CCxxx0_MDMCFG1,	0x42); //0x22-10k	 0x22						
	CC2500_WriteReg(CCxxx0_MDMCFG0,	0xF8); //0xf8        0xf8
	CC2500_WriteReg(CCxxx0_CHANNR,	0x00);
	CC2500_WriteReg(CCxxx0_DEVIATN,	0x01);	//0x44	// 00
	CC2500_WriteReg(CCxxx0_FREND1,	0x56);	// 0xB6   56������ʼ���
	CC2500_WriteReg(CCxxx0_FREND0,	0x10);
	CC2500_WriteReg(CCxxx0_MCSM1,	0x30);	// 0X00>NO CCA; 0X30>CCA modify by cy ����cca���޻����ڽ������ݰ��򱨾�
	CC2500_WriteReg(CCxxx0_MCSM0,	0x28);  //0x18
	CC2500_WriteReg(CCxxx0_FOCCFG,	0x15);	// 0x1D   Ƶ��ƫ�Ʋ���
	CC2500_WriteReg(CCxxx0_BSCFG,	0x6C);	// 0x1c   λͬ������
	CC2500_WriteReg(CCxxx0_AGCCTRL2,0x07);	// 0xc3    �������
	CC2500_WriteReg(CCxxx0_AGCCTRL1,0x38);	//modify by cy cca����ȡ��ǰRSSI+14db
	CC2500_WriteReg(CCxxx0_AGCCTRL0,0x91);	// 0xb2
	CC2500_WriteReg(CCxxx0_FSCAL3,	0xEA);
	CC2500_WriteReg(CCxxx0_FSCAL2,	0x0A);  //0x0a - 250k  0x06 --10k,0x08-2.4k
	CC2500_WriteReg(CCxxx0_FSCAL1,	0x00);	// ����Ƶ��ͬ��У׼
	CC2500_WriteReg(CCxxx0_FSCAL0,	0x11);
	CC2500_WriteReg(CCxxx0_FSTEST,	0x59);
	CC2500_WriteReg(CCxxx0_TEST2,	0x8F);	// 0x88
	CC2500_WriteReg(CCxxx0_TEST1,	0x21);	// 0x31
	CC2500_WriteReg(CCxxx0_TEST0,	0x0B);
	CC2500_WriteReg(CCxxx0_IOCFG2,	0x09);	//29 // CCA=0x09:1=free modify by cy����gdio2����ŵ��Ƿ�æ(æ0��1)
	CC2500_WriteReg(CCxxx0_IOCFG0,	0x01);	//0x01//0x07
	CC2500_WriteReg(CCxxx0_FIFOTHR,0x07);	
	CC2500_WriteReg(CCxxx0_PKTCTRL1,0x07);	//0x07//0x02//04
	CC2500_WriteReg(CCxxx0_PKTCTRL0,0x04);	//0x41 whiter+val	 //crc on 05						
	CC2500_WriteReg(CCxxx0_ADDR,	0x02);
	CC2500_WriteReg(CCxxx0_PKTLEN,	PACKET_LEN);
}

//��ʼ��CC2500
void CC2500_Init(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	u8 i = 0;

	CC2500_GPIO_Config();//��ʼ��CS,GDO0,GDO2
	SPI2_Init();//��ʼ��SPI

	POWER_UP_RESET_CCxxx0();
	CC2500_WriteRfSettings();
	CC2500_Select();
	CC2500_WriteByte(CCxxx0_PATABLE|WRITE_BURST);
	for(i=0;i<8;i++)
		CC2500_WriteByte(paTable_CC2500[i]);//����sleepģʽ��λ�����һ���������������Ҫ����д��
	CC2500_NoSelect();
	
	 /* Configure EXTI Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//���ñ����Ĺ��˵�ַ
void CC2500_SetRecvAddr(u8 SpecAddr)
{
	CC2500_WriteReg(CCxxx0_ADDR,SpecAddr);		
}

// ����WORģʽ
void CC2500_Enter_WOR(void)
{
  SpiStrobe(CCxxx0_SWORRST);
  SpiStrobe(CCxxx0_SWOR);
}

//��FIFO�ж����յ��İ�
u8  CC2500_ReceivePacket(u8 *rxBuffer)
{
    u8 i=0;
	CC2500_Select();
	CC2500_WriteByte(CCxxx0_RXFIFO_Muti);

	for(i=0;i<PACKET_LEN;i++)
	{
		rxBuffer[i] = CC2500_ReadByte();
	}
	
	while(CC2500_GET_GDO0())
		CC2500_ReadByte();
	
	CC2500_NoSelect(); 
	return PACKET_LEN;
}

//����һ����
u8 CC2500_SendPacket(u8 *txBuffer, u8 size)
{ 
	u8 i =0;
	
	CC2500_InterruptEnable(FALSE); //���ж�
	SpiStrobe(CCxxx0_SIDLE);
	SpiStrobe(CCxxx0_SFTX);//SFRX SFTXֻ����IDLEģʽ ִ�к�FIFOs�������������������
	CC2500_Select();
	CC2500_WriteByte(CCxxx0_TXFIFO_Muti);
    for(i=0;i<size;i++)
    	CC2500_WriteByte(txBuffer[i]);
	CC2500_NoSelect();
	SpiStrobe(CCxxx0_STX);
  	
	while(CC2500_ReadStatus(CCxxx0_MARCSTATE)!=0x01);//printf("%d\r\n",CC2500_ReadStatus(CCxxx0_MARCSTATE));
	CC2500_InterruptEnable(TRUE);//���ж�
	
    return 0;
}

//��ȡ��ǰ״̬����״̬
u8 CC2500_ReadStatus(u8 addr)
{
	u8  value;
	CC2500_Select();
	addr  |= READ_BURST;
	CC2500_WriteByte(addr);
	value = CC2500_ReadByte();
	CC2500_NoSelect();
	return value;
}

//��ȡRSSIֵ
u8 CC2500_ReadRSSIValue(void)
{
	return(CC2500_ReadStatus(0x34));
}

void CC2500_SeleChannel(u8 ChanID)
{
	CC2500_WriteReg(CCxxx0_CHANNR,	ChanID);
}

void CC2500_SetRxd(void)
{
	SpiStrobe(CCxxx0_SIDLE);
	SpiStrobe(CCxxx0_SFRX);
	//Delay_ms(1000);
	SpiStrobe(CCxxx0_SRX);
	//Delay_ms(1000);
}
