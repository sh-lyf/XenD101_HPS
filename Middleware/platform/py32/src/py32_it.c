/**
  ******************************************************************************
  * @file    py32_it.c
  * @author  liujinyang@puyasemi.com
  * @brief   interrupt services
  ******************************************************************************
  */
#include "py32f003_hal.h"
#include "platform.h"
#include "dataprocess.h"
#include "radar.h"

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1);
}

void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1);
}

void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1);
}

void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1);
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
	HAL_IncTick();
}

#ifdef SUPPORT_LOW_POWER
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle);
}
#endif

void SPI1_IRQHandler (void)
{
	HAL_SPI_IRQHandler(&SpiHandle);

}
void DMA1_Channel2_3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(SpiHandle.hdmarx);
}

void USART1_IRQHandler(void)
{

}
void USART2_IRQHandler(void)
{

}

void EXTI4_15_IRQHandler(void)
{
	if(LL_EXTI_IsActiveFlag_0_31(EXTI_IMR_IM6) != 0UL)
	{
        LL_EXTI_ClearFlag_0_31((EXTI_IMR_IM6));
    }
}


