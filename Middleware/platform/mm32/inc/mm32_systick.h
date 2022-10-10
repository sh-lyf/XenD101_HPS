/**
  ******************************************************************************
  * @file    mm32_systick.c
  * @author  ting.gao@iclegend.com
  * @brief   system tick Head file
  ******************************************************************************
  */

#ifndef __MM32_SYSTICK_H__
#define __MM32_SYSTICK_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "hal_conf.h"

#define TIMER3_PERIOD       (99U)

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
