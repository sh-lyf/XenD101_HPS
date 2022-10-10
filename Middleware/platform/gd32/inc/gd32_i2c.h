/**
  ******************************************************************************
  * @file    gd32_i2c.h
  * @author  ting.gao@iclegend.com
  * @brief   i2c header file
  ******************************************************************************
  */
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "gd32f3x0.h"

typedef enum  
{
	gd32_I2C_Speed_Config_100k = 100000U,
    gd32_I2C_Speed_Config_200k = 200000U,
    gd32_I2C_Speed_Config_300k = 300000U,
    gd32_I2C_Speed_Config_400k = 400000U,
    gd32_I2C_Speed_Config_1M = 1000000U,
}gd32_I2C_Speed_Config;

typedef enum  
{
	I2C_Speed_100k = 0,
    I2C_Speed_200k,
    I2C_Speed_300k,
    I2C_Speed_400k,
    I2C_Speed_1M,
}I2C_Speed;

#define I2C_SLAVE_ADDRESS7              (0x00)

void I2C_Init(uint8_t I2CSpeed);
uint16_t I2C_Write(uint16_t devAddr, uint8_t regAddr, uint16_t regVal);
uint16_t I2C_Read(uint16_t devAddr, uint8_t regAddr, uint16_t* regVal);

#ifdef __cplusplus
}
#endif

#endif

