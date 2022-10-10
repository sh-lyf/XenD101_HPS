/**
  ******************************************************************************
  * @file    iclm32_int.h
  * @author  iclm team
  * @brief   interrupt header file
  ******************************************************************************
  */
#ifndef __ICLM32_INT_H__
#define __ICLM32_INT_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "iclm32e2x_libopt.h"

typedef void (*irq_func_t)(void);
typedef struct IRQ 
{
    uint16_t    irq;
    uint8_t     priority;
    irq_func_t  function;
} IRQ_T;

#ifdef __cplusplus
}
#endif

#endif


