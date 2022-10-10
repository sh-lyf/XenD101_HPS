/**
  ******************************************************************************
  * @file    py32_gpio.c
  * @author  liujinyang@puyasemi.com
  * @brief   gpio services
  ******************************************************************************
  */

#include <stdio.h>
#include "global_conf.h"
#include "platform.h"


GPIO_TypeDef* LED_PORT[LEDn] = {LED0_GPIO_PORT};
uint16_t LED_PIN[LEDn] = {LED0_PIN};		 

static void PY32_LedInit(Led_TypeDef Led)
{	
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* Enable the GPIO_LED Clock */
	LEDx_GPIO_CLK_ENABLE(Led);

	/* Configure the GPIO_LED pin */
	GPIO_InitStruct.Pin = LED_PIN[Led];
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStruct);
	HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);

}

void LED_Init(void)
{
#ifdef PY32_EVAL_BOARD
    uint8_t i = 0;

    for (i = LED0; i < LED_MAX; i++)
    {
        PY32_LedInit((Led_TypeDef)i);
    }
    
    LED_Off(LED0);
#endif
}

void LED_On(Led_TypeDef Led)
{
#ifdef PY32_EVAL_BOARD
    if (Led >= LED_MAX)
    {
        return;
    }
    
    HAL_GPIO_WritePin(LED_PORT[Led],LED_PIN[Led],GPIO_PIN_SET);
#endif
}

void LED_Off(Led_TypeDef Led)
{
#ifdef PY32_EVAL_BOARD
    if (Led >= LED_MAX)
    {
        return;
    }

    HAL_GPIO_WritePin(LED_PORT[Led],LED_PIN[Led],GPIO_PIN_RESET);
#endif
}

void LED_Toggle(Led_TypeDef Led)
{
#ifdef PY32_EVAL_BOARD
    if (Led >= LED_MAX)
    {
        return;
    }

    if (HAL_GPIO_ReadPin(LED_PORT[Led],LED_PIN[Led]))
    {
        HAL_GPIO_WritePin(LED_PORT[Led],LED_PIN[Led],GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(LED_PORT[Led],LED_PIN[Led],GPIO_PIN_SET);
    }
#endif
}

#ifdef SUPPORT_ABD
void IoOutInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	IO_OUT_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = IO_OUT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(IO_OUT_PORT, &GPIO_InitStruct);
}

void IoOutOn(void)
{  
    HAL_GPIO_WritePin(IO_OUT_PORT, IO_OUT_PIN, GPIO_PIN_SET);
}

void IoOutOff(void)
{
    HAL_GPIO_WritePin(IO_OUT_PORT, IO_OUT_PIN, GPIO_PIN_RESET);
}

void TspiIoInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

	TSPI_IO_CLK_ENABLE();
    	
	GPIO_InitStruct.Pin = TSPI_IO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(TSPI_IO_PORT, &GPIO_InitStruct);
}

#endif


