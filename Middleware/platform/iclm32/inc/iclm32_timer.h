/**
  ******************************************************************************
  * @file    iclm32_timer.h
  * @author  ting.gao@iclegend.com
  * @brief   timer header file
  ******************************************************************************
  */

#ifndef ICLM32_TIMER_H
#define ICLM32_TIMER_H

#include "iclm32e2x_libopt.h"

#define RTC_TEAK_FREQ       (15000UL)

#define TICKS_PER_MILISECOND (RTC_TEAK_FREQ/1000)

void SYSTICK_Init(void);
void delay_1ms(uint32_t count);
void delay_decrement(void);

#endif
