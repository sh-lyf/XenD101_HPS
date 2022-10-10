/**
  ******************************************************************************
  * @file    mm32_iwdg.h
  * @author  ting.gao@iclegend.com
  * @brief   iwdg header file
  ******************************************************************************
  */

#ifndef __MM32_IWDG_H__
#define __MM32_IWDG_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "hal_conf.h"
#include "global_conf.h"

#define IWDG_FLAG_IVU          ((uint16_t)0x0004)
#define IWDG_RELOAD_VALUE      (1250U)   //reset time is about 0.8*1250 = 1s.
#define IWDG_INT_VALUE         (IWDG_RELOAD_VALUE - 82U)  //sleep time: 0.8*82 = 65.6ms

void IWDG_Init(void);
void IWDG_Reload(void);

#ifdef __cplusplus
}
#endif

#endif

