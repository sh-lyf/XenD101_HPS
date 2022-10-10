/**
  ******************************************************************************
  * @file    mm32_systick.c
  * @author  ting.gao@iclegend.com
  * @brief   system tick services
  ******************************************************************************
  */

#include "mm32_systick.h"
#include "system_mm32f0010.h"

volatile static uint32_t delay = 0;

void SYSTICK_Init(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000)) 
    {
        while (1);
    }
    NVIC_SetPriority(SysTick_IRQn, 0x0);
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds
    \param[out] none
    \retval     none
*/
void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay);
}

void delay_decrement(void)
{
    if (0U != delay)
    {
        delay--;
    }
}

//#ifdef SUPPORT_LOW_POWER
void TIM3_Enable(void)
{
    TIM_Cmd(TIM3, ENABLE);
}

void TIM3_Disable(void)
{
    TIM_Cmd(TIM3, DISABLE);
}

void TIM3_Init(void) /*1ms timer*/
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_StructInit;
    uint16_t prescaler = 0;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3, ENABLE);

    prescaler = RCC_GetSysClockFreq() / 100000 - 1, 
        
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = TIMER3_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
    //Sampling frequency of digital filter does not affect timer clock
    //Sampling Frequency Dividing Value
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    NVIC_StructInit.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_StructInit.NVIC_IRQChannelPriority = 1;
    NVIC_StructInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_StructInit);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    //TIM_Cmd(TIM3, ENABLE);
}
//#endif

