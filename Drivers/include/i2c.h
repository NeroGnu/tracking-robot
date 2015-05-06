#ifndef __I2C_H__
#define __I2C_H__
#include "stm32f4xx.h"

void I2C_Simu_GpioConfig(void);
bool I2C_Simu_Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data);
u8 I2C_Simu_Single_Read(u8 SlaveAddress,u8 REG_Address);

#endif
