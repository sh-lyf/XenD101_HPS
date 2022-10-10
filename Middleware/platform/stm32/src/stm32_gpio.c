/**
  ******************************************************************************
  * @file    stm32_gpio.c
  * @author  iclm team
  * @brief   gpio services
  ******************************************************************************
  */
#include "stm32_gpio.h"
#include <stdio.h>

static GPIO_TypeDef* GPIO_PORT[LED_MAX] = 
{
    LED0_GPIO_PORT,
    LED1_GPIO_PORT,
    LED2_GPIO_PORT,
    LED3_GPIO_PORT,
    LED4_GPIO_PORT
};

static uint16_t GPIO_PIN[LED_MAX] = 
{
    LED0_PIN,
    LED1_PIN,
    LED2_PIN, 
    LED3_PIN, 
    LED4_PIN
};


void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

 #if defined(STM32F429xx)
    __HAL_RCC_GPIOE_CLK_ENABLE();
 #elif defined(STM32H750xx)
    __HAL_RCC_GPIOD_CLK_ENABLE();
 #endif

    HAL_GPIO_WritePin(GPIO_PORT[LED0], LED0_PIN|LED1_PIN|LED2_PIN|LED3_PIN 
                            |LED4_PIN, GPIO_PIN_RESET);
    
    GPIO_InitStruct.Pin = LED0_PIN|LED1_PIN|LED2_PIN|LED3_PIN 
                            |LED4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIO_PORT[LED0], &GPIO_InitStruct);

}

void LED_On(led_typedef_enum lednum)
{
    HAL_GPIO_WritePin(GPIO_PORT[lednum], GPIO_PIN[lednum], GPIO_PIN_SET);
}

void LED_Off(led_typedef_enum lednum)
{
    HAL_GPIO_WritePin(GPIO_PORT[lednum], GPIO_PIN[lednum], GPIO_PIN_RESET);
}

void LED_Toggle(led_typedef_enum lednum)
{
    HAL_GPIO_TogglePin(GPIO_PORT[lednum], GPIO_PIN[lednum]);
}

//#ifdef SUPPORT_TSPI_IO
void ABD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();

    //PA7     ------> ABD status: high--active; low--inactive
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t ABD_GetStatus(void)
{
    GPIO_PinState status = GPIO_PIN_RESET;
    
    status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);

    return status;
}

void ABD_PrintStatus(void)
{
    static uint16_t tickCnt = 0;
    uint8_t status = 0;

    tickCnt++;
    if (tickCnt % ABD_CHECK_INTERVAL == 0)
    {
        status = ABD_GetStatus();
        if (status)
        {
            printf("ABD status: ON\r\n");
        }
        else
        {
            printf("ABD status: ---------------OFF----------------\r\n");
        }
    }
}
//#endif

void GPIO_PowerCtlPinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void GPIO_PowerCtlOn(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
}

void GPIO_PowerCtlOff(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
}


