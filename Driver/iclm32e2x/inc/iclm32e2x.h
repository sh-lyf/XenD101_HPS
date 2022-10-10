 /**
   ******************************************************************************
   * @file    iclm32e2x.h
   * @author  ting.gao@iclegend.com
   * @brief   iclm32 e2x header file
   ******************************************************************************
   */
#ifndef __ICLM32E2X_H__
#define __ICLM32E2X_H__

#ifdef cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include <stdio.h>

#define GPIO_BASE                    (0x60002000UL)
#define SYS_INT_BASE                 (0x60001000UL)
#define UART_BASE                    (0x60004000UL)

#define REG32(addr)                  (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr)                  (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr)                   (*(volatile uint8_t *)(uint32_t)(addr))
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))

#ifdef cplusplus
}
#endif
#endif
