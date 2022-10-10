/**
  ******************************************************************************
  * @file    py32_i2c.h
  * @author  liujinyang@puyasemi.com
  * @brief   i2c header file
  ******************************************************************************
  */
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f003_hal_conf.h"
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

/* Private defines -----------------------------------------------------------*/
#define I2Cx                            I2C
#define I2Cx_CLK_ENABLE()               __HAL_RCC_I2C_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE() 

#define I2Cx_FORCE_RESET()              __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()            __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_1
#define I2Cx_SCL_GPIO_PORT              GPIOF
#define I2Cx_SCL_AF                     GPIO_AF12_I2C

#define I2Cx_SDA_PIN                    GPIO_PIN_0
#define I2Cx_SDA_GPIO_PORT              GPIOF
#define I2Cx_SDA_AF                     GPIO_AF12_I2C

#define I2C_ADDRESS        				(I2C_ADDR_RADAR_Chip0)
#define I2C_DUTYCYCLE    				I2C_DUTYCYCLE_2//I2C_DUTYCYCLE_2

#define I2C_TIMEOUT                     (100U)

void I2C_Init(uint8_t I2CSpeed);
uint16_t I2C_Write(uint16_t devAddr, uint8_t regAddr, uint16_t regVal);
uint16_t I2C_Read(uint16_t devAddr, uint8_t regAddr, uint16_t* regVal);

#ifdef __cplusplus
}
#endif

#endif

