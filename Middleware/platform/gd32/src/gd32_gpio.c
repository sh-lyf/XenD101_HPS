/**
  ******************************************************************************
  * @file    gd32_gpio.c
  * @author  ting.gao@iclegend.com
  * @brief   gpio services
  ******************************************************************************
  */
#include "gd32_gpio.h"

static uint32_t GPIO_PORT[LED_MAX] = 
{
    LED0_GPIO_PORT,
    LED1_GPIO_PORT,
    LED2_GPIO_PORT,
    LED3_GPIO_PORT
};

static uint32_t GPIO_PIN[LED_MAX] = 
{
    LED0_PIN,
    LED1_PIN,
    LED2_PIN, 
    LED3_PIN
};

static rcu_periph_enum GPIO_CLK[LED_MAX] = 
{
    LED0_GPIO_CLK,
    LED1_GPIO_CLK,
    LED2_GPIO_CLK, 
    LED3_GPIO_CLK
};
   
void GD32F3X0_LedInit(led_typedef_enum lednum)
{
    rcu_periph_clock_enable(GPIO_CLK[lednum]);
   
    gpio_mode_set(GPIO_PORT[lednum], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN[lednum]);
    gpio_output_options_set(GPIO_PORT[lednum], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN[lednum]);

    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

void LED_Init(void)
{
    uint8_t i = 0;

    /*LED is for gd32f350 eval board*/
    for (i = LED0; i < LED_MAX; i++)
    {
        GD32F3X0_LedInit((led_typedef_enum)i);
    }
}

void LED_On(led_typedef_enum lednum)
{
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

void LED_Off(led_typedef_enum lednum)
{
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

void LED_Toggle(led_typedef_enum lednum)
{
    GPIO_TG(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

