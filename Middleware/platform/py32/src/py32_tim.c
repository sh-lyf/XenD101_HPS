/**
  ******************************************************************************
  * @file    py32_tim.c
  * @author  liujinyang@puyasemi.com
  * @brief   timer services
  ******************************************************************************
  */

#include "platform.h"
#include "radar.h"

TIM_HandleTypeDef    TimHandle;
uint32_t uwPrescalerValue = 0;

//#ifdef SUPPORT_LOW_POWER

void TIM3_Enable(void)
{
    if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
    {
        /* Starting Error */
        while(1);
    }
}

void TIM3_Disable(void)
{
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	/*##-1- Enable peripheral clock #################################*/
	/* TIMx Peripheral clock enable */
	TIMx_CLK_ENABLE();

	/*##-2- Configure the NVIC for TIMx ########################################*/
	/* Set the TIMx priority */
	HAL_NVIC_SetPriority(TIMx_IRQn, 3, 0);

	/* Enable the TIMx global Interrupt */
	HAL_NVIC_EnableIRQ(TIMx_IRQn);
}

void TIM3_Init(void) /*1ms timer*/
{
	//100KHz
    uwPrescalerValue = (uint32_t)(SystemCoreClock / 100000) - 1;
    /* Set TIMx instance */
    TimHandle.Instance = TIMx;
	//1ms timer
    TimHandle.Init.Period            = TIMER3_PERIOD;
    TimHandle.Init.Prescaler         = uwPrescalerValue;
    TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
        /* Initialization Error */
        while(1);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	#ifdef PY32_EVAL_BOARD
    LED_Toggle(LED0);
	#else
	static uint16_t timer3Cnt = 0;

    timer3Cnt++;
    if (timer3Cnt >= POWER_DOWN_PERIOD)
    {
        TIM3_Disable();
        timer3Cnt = 0;
        Radar_EnterNormalMode();
    }
	#endif
}
//#endif

