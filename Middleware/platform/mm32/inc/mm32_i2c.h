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

#include "hal_conf.h"
#include "global_conf.h"

typedef enum  
{
	I2C_Speed_Config_100k = 100000U,
    I2C_Speed_Config_200k = 200000U,
    I2C_Speed_Config_300k = 300000U,
    I2C_Speed_Config_400k = 400000U,
}I2C_Speed_Config;

typedef enum  
{
	I2C_Speed_100k = 0,
    I2C_Speed_200k,
    I2C_Speed_300k,
    I2C_Speed_400k
}I2C_Speed;

#define I2C1_SCL_BUSCLK                  RCC_AHBENR_GPIOA
#define I2C1_SCL_PIN                     GPIO_Pin_5
#define I2C1_SCL_PORT                    GPIOA
#define I2C1_SCL_AFSOURCE                GPIO_PinSource5
#define I2C1_SCL_AFMODE                  GPIO_AF_3

#define I2C1_SDA_BUSCLK                  RCC_AHBENR_GPIOA
#define I2C1_SDA_PIN                     GPIO_Pin_4
#define I2C1_SDA_PORT                    GPIOA
#define I2C1_SDA_AFSOURCE                GPIO_PinSource4
#define I2C1_SDA_AFMODE                  GPIO_AF_3

void I2C_Init(uint8_t I2CSpeed);
uint16_t I2C_Write(uint16_t devAddr, uint8_t regAddr, uint16_t regVal);
uint16_t I2C_Read(uint16_t devAddr, uint8_t regAddr, uint16_t* regVal);

#ifdef __cplusplus
}
#endif

#endif

