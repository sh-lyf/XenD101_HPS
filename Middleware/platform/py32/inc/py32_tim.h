/**
  ******************************************************************************
  * @file    py32_tim.h
  * @author  liujinyang@puyasemi.com
  * @brief   timer header file
  ******************************************************************************
  */

#ifndef __PY32_TIM_H__
#define __PY32_TIM_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "py32f003_hal_conf.h"

#define TIMER3_PERIOD       		(99U)
#define TIMx                        TIM3
#define TIMx_CLK_ENABLE()           __HAL_RCC_TIM3_CLK_ENABLE()
#define TIMx_IRQn                   TIM3_IRQn

extern TIM_HandleTypeDef    TimHandle;

void SYSTICK_Init(void);
void delay_1ms(uint32_t count);
void delay_decrement(void);
void TIM3_Enable(void);
void TIM3_Disable(void);
void TIM3_Init(void);

#ifdef __cplusplus
}
#endif

#endif
