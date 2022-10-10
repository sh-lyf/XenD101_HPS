/**
  ******************************************************************************
  * @file    py32_i2c.c
  * @author  liujinyang@puyasemi.com
  * @brief   i2c services
  ******************************************************************************
  */
  
#include "platform.h"
#include "utilities.h"
#include "radar.h"

static I2C_HandleTypeDef I2cHandle;

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* USER CODE END I2C1_MspInit 0 */
    I2Cx_CLK_ENABLE(); 
    I2Cx_SDA_GPIO_CLK_ENABLE();
	I2Cx_SDA_GPIO_CLK_ENABLE();

    /**I2C1 GPIO Configuration    
    PF1     ------> I2C1_SCL
    PF0     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = I2Cx_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = I2Cx_SCL_AF;
    HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);
	
    GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
    GPIO_InitStruct.Alternate = I2Cx_SDA_AF;
    HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);

	__HAL_RCC_I2C_FORCE_RESET();
	__HAL_RCC_I2C_RELEASE_RESET();
}

static void PY32_I2C_Init(uint32_t speed)
{
	/*##-1- Configure the I2C peripheral ######################################*/

	I2cHandle.Instance             = I2Cx;
	I2cHandle.Init.ClockSpeed      = speed;
	I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE;
	I2cHandle.Init.OwnAddress1     = 0;                                                                                                                         
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.OwnAddress2     = 0;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;  

	if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
	{
		while(1);
	}
}

static HAL_StatusTypeDef PY32_I2C_Write(uint16_t devAddr, uint8_t regAddr, uint16_t regVal)
{
    regVal = LE16_TO_BIG(regVal);

    return HAL_I2C_Mem_Write(&I2cHandle, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&regVal, sizeof(regVal), I2C_TIMEOUT);
}


static HAL_StatusTypeDef PY32_I2C_Read(uint16_t devAddr, uint8_t regAddr, uint16_t* regVal)
{  
	uint16_t val = 0;
	uint8_t retryCount = 3;
	HAL_StatusTypeDef result = HAL_OK;
	while(retryCount--)
	{
		result = HAL_I2C_Mem_Read(&I2cHandle, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&val,  sizeof(val), I2C_TIMEOUT);
		if( result == HAL_OK)
		{
			break;
		}
		else
		{
			HAL_Delay(1);
		}
	}
	*regVal = BIG16_TO_LE(val);
    return result;
}

static uint32_t I2C_GetConfigBySpeed(uint8_t I2CSpeed)
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

void I2C_Init(uint8_t I2CSpeed)
{
    uint32_t ConfigSpeed = 0;
    
    ConfigSpeed = I2C_GetConfigBySpeed(I2CSpeed);
	PY32_I2C_Init(ConfigSpeed);
}

uint16_t I2C_Write(uint16_t devAddr, uint8_t regAddr, uint16_t regVal)
{
    //regVal = LE16_TO_BIG(regVal);
    HAL_StatusTypeDef status = HAL_OK;
	status = PY32_I2C_Write(devAddr,regAddr, regVal);
    if(status != HAL_OK)
	{
		printf("PY32_I2C_Write Error,status is 0x%02x\r\n",status);
		//while(1);
	}

	return 0;
}


uint16_t I2C_Read(uint16_t devAddr, uint8_t regAddr, uint16_t* regVal)
{  
    uint16_t val = 0;
     HAL_StatusTypeDef status = HAL_OK;
	 status = PY32_I2C_Read(devAddr,regAddr, &val);
    if(status != HAL_OK)
	{
		printf("PY32_I2C_Read Error\r\n");
		//while(1);

	}

    //*regVal = BIG16_TO_LE(val);
	*regVal = val;

	return 0;
}
