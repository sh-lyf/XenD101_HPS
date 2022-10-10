/**
  ******************************************************************************
  * @file    iclm32_timer.c
  * @author  ting.gao@iclegend.com
  * @brief   timer services
  ******************************************************************************
  */

#include "iclm32_timer.h"
#include "iclm32_int.h"
#include "iclm32_gpio.h"

volatile static uint32_t delay = 0;

void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay){
    }
}

void delay_decrement(void)
{
    if (0U != delay){
        delay--;
    }
}

void __attribute__((interrupt))  handle_timer_interrupt(void)
{
    delay_decrement();
    schedule_timer_interrupt(TICKS_PER_MILISECOND);
}

void SYSTICK_Init(void)
{
    IRQ_T irqTimer = {0};

    irqTimer.function = &handle_timer_interrupt;
    irqTimer.irq = INTERRUPT_ID_TMR;
    irqTimer.priority = 3;

    schedule_timer_interrupt(TICKS_PER_MILISECOND);
    INT_Request(&irqTimer);
}



