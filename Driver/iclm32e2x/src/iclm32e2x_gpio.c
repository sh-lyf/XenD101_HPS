/**
  ******************************************************************************
  * @file    iclm32e2x_gpio.c
  * @author  ting.gao@iclegend.com
  * @brief   iclm32 e2x gpio driver
  ******************************************************************************
  */
  
#include "iclm32e2x_gpio.h"

static uint32_t gpioOutValue = 0x00000000;
static uint32_t gpioMode = 0x00000000;
static uint32_t gpioIrqStatus = 0x00000000;

void gpio_mode_set(uint32_t gpio_periph, uint32_t mode,  uint32_t pin)
{
    uint16_t i = 0;

    for(i = 0U;i < GPIO_PIN_MAX;i++)
    {
        if((1U << i) & pin)
        {
            gpioMode &= ~BIT(i);
            if (GPIO_MODE_OUTPUT == mode)
            {
                gpioMode |= BIT(i);
            }
        }
    }

    GPIO_OUT_ENABLE(gpio_periph) = gpioMode;
}

void gpio_out_value(uint32_t gpio_periph, uint32_t value, uint32_t pin)
{
    uint16_t i = 0;

    for(i = 0U;i < GPIO_PIN_MAX;i++)
    {
        if((1U << i) & pin)
        {
            gpioOutValue &= ~BIT(i);
            if (value)
            {
                gpioOutValue |= BIT(i);
            }
        }
    }

    GPIO_OUT_VALUE(gpio_periph) = gpioOutValue;
}

void gpio_irq_set(uint32_t gpio_periph, uint32_t status, uint32_t pin)
{
    uint16_t i = 0;

    for(i = 0U;i < GPIO_PIN_MAX;i++)
    {
        if((1U << i) & pin)
        {
            gpioIrqStatus &= ~BIT(i);
            if (status)
            {
                gpioIrqStatus |= BIT(i);
            }
        }
    }

    GPIO_IRQ_ENABLE(gpio_periph) = gpioIrqStatus;
}


