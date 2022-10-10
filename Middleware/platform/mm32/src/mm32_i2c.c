/**
  ******************************************************************************
  * @file    gd32_i2c.c
  * @author  ting.gao@iclegend.com
  * @brief   i2c services
  ******************************************************************************
  */
#include "mm32_i2c.h"
#include "utilities.h"
#include "radar.h"

static void MM32_I2cWrite(uint8_t regAddr, uint8_t* pBuffer,  uint8_t size)
{
    I2C_SendData(I2C1, regAddr);
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);
    
    while (size--)
    {
        I2C_SendData(I2C1, *pBuffer);
        while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);
        pBuffer++;
    }
   
    I2C_GenerateSTOP(I2C1, ENABLE);
    while((I2C_GetITStatus(I2C1, I2C_IT_STOP_DET)) == 0);
}

static void MM32_I2cRead(uint8_t regAddr, uint8_t* pBuffer,  uint8_t size)
{  
    I2C_SendData(I2C1, regAddr);
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);

    I2C_GenerateSTOP(I2C1, ENABLE);
    while((I2C_GetITStatus(I2C1, I2C_IT_STOP_DET)) == 0);

    while (size--) 
    {
        I2C_ReadCmd(I2C1);
        while (I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);
    
        while (I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_RFNE) == 0);
        *pBuffer = I2C_ReceiveData(I2C1);
        pBuffer++;
    }

}

uint16_t I2C_Write(uint16_t devAddr, uint8_t regAddr, uint16_t regVal)
{
    regVal = LE16_TO_BIG(regVal);
    
    MM32_I2cWrite(regAddr, (uint8_t*)&regVal,  sizeof(regVal));
	
	return 0;
}


uint16_t I2C_Read(uint16_t devAddr, uint8_t regAddr, uint16_t* regVal)
{  
    uint16_t val = 0;
    
    MM32_I2cRead(regAddr, (uint8_t*)&val,  sizeof(val));

    *regVal = BIG16_TO_LE(val);
	return 0;
}

uint32_t I2C_GetConfigBySpeed(uint8_t I2CSpeed)
{
	uint32_t Config = I2C_Speed_Config_400k;
    
	switch(I2CSpeed)
	{
		case I2C_Speed_100k:
		{
			Config = I2C_Speed_Config_100k;
			break;
		}
		case I2C_Speed_200k:
		{
			Config = I2C_Speed_Config_200k;
			break;
		}
		case I2C_Speed_300k:
		{
			Config = I2C_Speed_Config_300k;
			break;
		}
		case I2C_Speed_400k:
		{
			Config = I2C_Speed_Config_400k;
			break;
		}
		default:
			break;
	}
    
	return Config;
}

static void I2C1_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(I2C1_SDA_BUSCLK, ENABLE);
    RCC_AHBPeriphClockCmd(I2C1_SCL_BUSCLK, ENABLE);
    GPIO_PinAFConfig(I2C1_SCL_PORT, I2C1_SCL_AFSOURCE, I2C1_SCL_AFMODE);
    GPIO_PinAFConfig(I2C1_SDA_PORT, I2C1_SDA_AFSOURCE, I2C1_SDA_AFMODE);

    GPIO_InitStructure.GPIO_Pin  = I2C1_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = I2C1_SDA_PIN;
    GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);
}

void I2C_MasterModeInit(I2C_TypeDef* I2Cx, u32 uiI2C_speed)
{
    I2C_InitTypeDef I2C_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    I2C_StructInit(&I2C_InitStructure);
    I2C_InitStructure.Mode = I2C_CR_MASTER;
    I2C_InitStructure.OwnAddress = 0;
    I2C_InitStructure.Speed = I2C_CR_FAST;
    I2C_InitStructure.ClockSpeed = uiI2C_speed;

    HAL_I2C_Init(I2Cx, &I2C_InitStructure);
    I2C_Cmd(I2Cx, ENABLE);
}

void I2C_SetDeviceAddr(I2C_TypeDef* I2Cx, u8 deviceaddr)
{
    I2C_Cmd(I2Cx, DISABLE);
    I2C_Send7bitAddress(I2Cx, deviceaddr, I2C_Direction_Transmitter);
    I2C_Cmd(I2Cx, ENABLE);
}

void I2C_Init(uint8_t I2CSpeed)
{
    uint32_t ConfigSpeed = 0;
    
    I2C1_GPIO_Config();
    ConfigSpeed = I2C_GetConfigBySpeed(I2CSpeed);
    I2C_MasterModeInit(I2C1, ConfigSpeed);
    I2C_SetDeviceAddr(I2C1, I2C_ADDR_RADAR_Chip0);
}


