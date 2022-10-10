/**
  ******************************************************************************
  * @file    mm32_iwdg.c
  * @author  ting.gao@iclegend.com
  * @brief   iwdg services
  ******************************************************************************
  */
#include "mm32_iwdg.h"

void WWDG_IWDG_IRQHandler(void)
{
    EXTI_ClearFlag(EXTI_Line17);
    IWDG_WriteAccessCmd(0x5555);
    IWDG->CR |= 0x02;
    IWDG->KR = 0xAAAA;

    //printf("iwdg\r\n");
}

void IVU_CheckStatus(void)
{
    while(1) 
    {
        if(IWDG_GetFlagStatus(IWDG_FLAG_IVU) == RESET) 
        {
            break;
        }
    }
}

static void Write_Iwdg_ON(u16 IWDG_Prescaler, u16 Reload)
{
    //Start the internal low-speed clock and wait for the clock to be ready
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    //Setting Clock Pre-division Frequency
    PVU_CheckStatus();
    IWDG_WriteAccessCmd(0x5555);
    IWDG_SetPrescaler(IWDG_Prescaler);

    //Setting overload register values
    RVU_CheckStatus();
    IWDG_WriteAccessCmd(0x5555);
    IWDG_SetReload(Reload & 0xfff);
#ifdef MCU_LOW_POWER
    IVU_CheckStatus();
    IWDG_WriteAccessCmd(0x5555);
    IWDG->IGEN = IWDG_INT_VALUE;
#endif
    //Loading and Enabling Counter
    IWDG_ReloadCounter();
#ifdef MCU_LOW_POWER
    IWDG_WriteAccessCmd(0x5555);
    IWDG->CR |= 1;
#endif
    IWDG_Enable();
}

void IWDG_Reload(void)
{
    IWDG_ReloadCounter();
}

void Iwdg_Irq_ON(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    EXTI_DeInit();
    EXTI_StructInit(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17 ;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt ;
    EXTI_InitStructure.EXTI_Trigger =   EXTI_Trigger_Rising_Falling  ;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_ClearITPendingBit(EXTI_Line17);

    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void IWDG_Init(void)
{
#ifdef MCU_LOW_POWER
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWREN, ENABLE);
    Iwdg_Irq_ON();
#endif
    Write_Iwdg_ON(IWDG_Prescaler_32, IWDG_RELOAD_VALUE); 
}

