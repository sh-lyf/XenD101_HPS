/**
  ******************************************************************************
  * @file    mm32_gpio.c
  * @author  ting.gao@iclegend.com
  * @brief   gpio services
  ******************************************************************************
  */

#include <stdio.h>
#include "global_conf.h"
#include "mm32_gpio.h"

static GPIO_TypeDef* GPIO_PORT[LED_MAX] = 
{
    LED0_GPIO_PORT
};

static uint32_t GPIO_PIN[LED_MAX] = 
{
    LED0_PIN
};

static uint32_t GPIO_CLK[LED_MAX] = 
{
    LED0_GPIO_CLK
};
   
void MM32_LedInit(led_typedef_enum lednum)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(GPIO_CLK[lednum], ENABLE);
    
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  =  GPIO_PIN[lednum];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_PORT[lednum], &GPIO_InitStructure);
}

void LED_Init(void)
{
#ifdef MM32_EVAL_BOARD
    uint8_t i = 0;

    for (i = LED0; i < LED_MAX; i++)
    {
        MM32_LedInit((led_typedef_enum)i);
    }
    
    LED_Off(LED0);
#endif
}

//#define LED1_ON()  GPIO_ResetBits(LED1_Port,LED1_Pin)
//#define LED1_OFF()  GPIO_SetBits(LED1_Port,LED1_Pin)
//#define LED1_TOGGLE()  (GPIO_ReadOutputDataBit(LED1_Port,LED1_Pin))?(GPIO_ResetBits(LED1_Port,LED1_Pin)):(GPIO_SetBits(LED1_Port,LED1_Pin))

void LED_On(led_typedef_enum lednum)
{
#ifdef MM32_EVAL_BOARD
    if (lednum >= LED_MAX)
    {
        return;
    }
    
    GPIO_ResetBits(GPIO_PORT[lednum],GPIO_PIN[lednum]);
#endif
}

void LED_Off(led_typedef_enum lednum)
{
#ifdef MM32_EVAL_BOARD
    if (lednum >= LED_MAX)
    {
        return;
    }

    GPIO_SetBits(GPIO_PORT[lednum],GPIO_PIN[lednum]);
#endif
}

void LED_Toggle(led_typedef_enum lednum)
{
#ifdef MM32_EVAL_BOARD
    if (lednum >= LED_MAX)
    {
        return;
    }

    if (GPIO_ReadOutputDataBit(GPIO_PORT[lednum],GPIO_PIN[lednum]))
    {
        GPIO_ResetBits(GPIO_PORT[lednum],GPIO_PIN[lednum]);
    }
    else
    {
        GPIO_SetBits(GPIO_PORT[lednum],GPIO_PIN[lednum]);
    }
#endif
}

#ifdef SUPPORT_ABD
void IoOutInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = IO_OUT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(IO_OUT_PORT, &GPIO_InitStructure);
}

void IoOutOn(void)
{  
    GPIO_SetBits(IO_OUT_PORT, IO_OUT_PIN);
}

void IoOutOff(void)
{
    GPIO_ResetBits(IO_OUT_PORT, IO_OUT_PIN);
}

void TspiIoInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

#ifdef SUPPORT_NEW_PIN
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);
#else
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
#endif
    
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = TSPI_IO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(TSPI_IO_PORT, &GPIO_InitStructure);
}

void ExtixInit(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SYSCFG, ENABLE);

    //PA.11 use EXTI line 11
    SYSCFG_EXTILineConfig(TSPI_EXTI_PORT_SOURCE, TSPI_EXTI_PIN_SOURCE);

    EXTI_InitStructure.EXTI_Line = TSPI_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TSPI_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
#endif

void GPIO_PowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(PWCTL_GPIO_CLK, ENABLE);
    
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  =  PWCTL_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PWCTL_GPIO_PORT, &GPIO_InitStructure);
}

void GPIO_PowerCtlOn(void)
{
    GPIO_SetBits(PWCTL_GPIO_PORT, PWCTL_GPIO_PIN);
}

void GPIO_PowerCtlOff(void)
{
    GPIO_ResetBits(PWCTL_GPIO_PORT, PWCTL_GPIO_PIN);
}



