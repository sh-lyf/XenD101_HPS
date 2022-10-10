/**
  ******************************************************************************
  * @file    iclm32e2x_gpio.h
  * @author  ting.gao@iclegend.com
  * @brief   iclm32 e2x gpio header file
  ******************************************************************************
  */
#ifndef __ICLM32E2X_GPIO_H__
#define __ICLM32E2X_GPIO_H__

#include "iclm32e2x.h"

#define GPIOA                               (GPIO_BASE + 0x00000000UL)

#define GPIO_OUT_VALUE(gpiox)               REG32((gpiox) + 0x00000000UL)    
#define GPIO_OUT_ENABLE(gpiox)              REG32((gpiox) + 0x00000004UL) 
#define GPIO_IRQ_ENABLE(gpiox)              REG32((gpiox) + 0x0000000CUL)

#define GPIO_MODE_OUTPUT                    (1U)
#define GPIO_MODE_INPUT                     (0U) 
#define GPIO_SET                            (1U)
#define GPIO_UNSET                          (0U)
#define GPIO_INT_ENABLE                     (1U)
#define GPIO_INT_DISABLE                    (0U)

#define GPIO_PIN_0                          BIT(0)                
#define GPIO_PIN_1                          BIT(1)                
#define GPIO_PIN_2                          BIT(2)               
#define GPIO_PIN_3                          BIT(3)                
#define GPIO_PIN_4                          BIT(4)                
#define GPIO_PIN_5                          BIT(5)                
#define GPIO_PIN_6                          BIT(6)                
#define GPIO_PIN_7                          BIT(7)                
#define GPIO_PIN_MAX                        (8U)

void gpio_mode_set(uint32_t gpio_periph, uint32_t mode,  uint32_t pin);
void gpio_out_value(uint32_t gpio_periph, uint32_t value, uint32_t pin);
void gpio_irq_set(uint32_t gpio_periph, uint32_t status, uint32_t pin);

#endif
