/**
  ******************************************************************************
  * @file    iclm32e2x_gpio.h
  * @author  ting.gao@iclegend.com
  * @brief   iclm32 e2x gpio header file
  ******************************************************************************
  */

#ifndef __ICLM32_UART_H__
#define __ICLM32_UART_H__

#include "iclm32e2x.h"

#define UART0                        (UART_BASE + 0x000U)

#define USART_CTL0(uartx)            REG32((uartx) + 0x04U)

#endif
