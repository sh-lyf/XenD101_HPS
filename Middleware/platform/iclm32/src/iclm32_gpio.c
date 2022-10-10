/**
  ******************************************************************************
  * @file    gd32_gpio.c
  * @author  ting.gao@iclegend.com
  * @brief   gpio services
  ******************************************************************************
  */
#include "iclm32_gpio.h"
#include "iclm32_int.h"


static uint32_t GPIO_PORT[LED_MAX] = 
{
    LED0_GPIO_PORT,
    LED1_GPIO_PORT
};

static uint32_t GPIO_PIN[LED_MAX] = 
{
    LED0_PIN,
    LED1_PIN
};

void LED_Init(void)
{
    led_typedef_enum i = 0;

    for (i = LED0; i < LED_MAX; i++)
    {
        gpio_mode_set(GPIO_PORT[i], GPIO_MODE_OUTPUT, GPIO_PIN[i]);
        gpio_out_value(GPIO_PORT[i], GPIO_SET, GPIO_PIN[i]);
    }
}

void LED_On(led_typedef_enum lednum)
{
    gpio_out_value(GPIO_PORT[lednum], GPIO_UNSET, GPIO_PIN[lednum]);
}

void LED_Off(led_typedef_enum lednum)
{
    gpio_out_value(GPIO_PORT[lednum], GPIO_SET, GPIO_PIN[lednum]);
}

void LED_Toggle(led_typedef_enum lednum)
{
    static uint8_t val = 0;

    val ^= 1;
    gpio_out_value(GPIO_PORT[lednum], val, GPIO_PIN[lednum]);
}

void __attribute__((interrupt))  handle_lc1_interrupt(void)
{
    static int cnt = 0;

    if (get_system_int_status(SYS_INT_LOCAL_1) & SYS_INT_ID_7)
    {
        cnt++;
        if (cnt % 2 == 0)
        {
            LED_Off(LED1);
        }
        else
        {
            LED_On(LED1);
        }
    }

    system_int_clear_status(SYS_INT_LOCAL_1, SYS_INT_ID_7);
}

void GPIO_IntTest(void)
{
    IRQ_T irqTimer = {0};
    
    gpio_irq_set(GPIOA, GPIO_INT_ENABLE, GPIO_PIN_7);
    system_int_set(SYS_INT_LOCAL_1, INT_ENABLE, SYS_INT_ID_7);

    irqTimer.function = &handle_lc1_interrupt;
    irqTimer.irq = INTERRUPT_ID_LC1;
    irqTimer.priority = 3;

    INT_Request(&irqTimer);
}


