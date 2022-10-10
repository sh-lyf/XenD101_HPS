/**
  ******************************************************************************
  * @file    py32_pwr.h
  * @author  liujinyang@puyasemi.com
  * @brief   power header file
  ******************************************************************************
  */

#ifndef __PY32_PWR_H__
#define __PY32_PWR_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "py32f003_hal_conf.h"

#define WAKEUP_IO_PORT           GPIOA           
#define WAKEUP_IO_PIN            GPIO_PIN_6

void PWR_WakeUpInit(void);
void PWR_EnterSleepMode(void);
void PWR_EnterStopMode(void);


#ifdef __cplusplus
}
#endif

#endif
