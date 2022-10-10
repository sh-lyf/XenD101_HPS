/**
  ******************************************************************************
  * @file    stm32_i2c.h
  * @author  iclm team
  * @brief   i2c header file
  ******************************************************************************
  */
#ifndef __STM32_I2C_H__
#define __STM32_I2C_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#if defined(STM32F429xx)
#include "stm32f4xx_hal.h"
#endif

#if defined(STM32H750xx)
#include "stm32h7xx_hal.h"
#endif

#if defined(STM32F429xx)
#define I2Cx                            I2C2
typedef enum  
{
	stm32_I2C_Speed_Config_100k = 100000U,
    stm32_I2C_Speed_Config_200k = 200000U,
    stm32_I2C_Speed_Config_300k = 300000U,
    stm32_I2C_Speed_Config_400k = 400000U,
    stm32_I2C_Speed_Config_1M = 1000000U,
}stm32_I2C_Speed_Config;
#endif

#if defined(STM32H750xx)
#define I2Cx                            I2C4
typedef enum  
{
	stm32_I2C_Speed_Config_100k = 0x401007B8,
    stm32_I2C_Speed_Config_200k = 0x0090E8E6,
    stm32_I2C_Speed_Config_300k = 0x009041E6,
    stm32_I2C_Speed_Config_400k = 0x00C018BC,
    stm32_I2C_Speed_Config_1M = 0x00901826,
}stm32_I2C_Speed_Config;
#endif

#define I2C_TIMEOUT                     (100U)

typedef enum  
{
	I2C_Speed_100k = 0,
    I2C_Speed_200k,
    I2C_Speed_300k,
    I2C_Speed_400k,
    I2C_Speed_1M,
}I2C_Speed;

void I2C_Init(uint8_t I2CSpeed);
HAL_StatusTypeDef I2C_Write(uint16_t devAddr, uint8_t regAddr, uint16_t regVal);
HAL_StatusTypeDef I2C_Read(uint16_t devAddr, uint8_t regAddr, uint16_t* regVal);
void I2C_ResetSpeed(uint8_t I2CSpeed);

#ifdef __cplusplus
}
#endif

#endif

